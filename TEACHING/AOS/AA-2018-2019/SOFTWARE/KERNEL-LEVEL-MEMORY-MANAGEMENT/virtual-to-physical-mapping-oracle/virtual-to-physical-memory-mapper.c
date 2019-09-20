/*
* 
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License as published by the Free Software
* Foundation; either version 3 of the License, or (at your option) any later
* version.
* 
* This module is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
* A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* 
* @file virtual-to-physical-memory-mapper.c 
* @brief This is the main source for the Linux Kernel Module which implements
*       a system call that can be used to query the kernel for current mappings of virtual pages to 
*	physical frames - this service is x86_64 specific in the curent implementation
	NOTE: it is a baseline that does not fully comply with atomicity requirements in concurrent kernel 
	level memory map management!!!!
*
* @author Francesco Quaglia
*
* @date March 3, 2017
*/

#define EXPORT_SYMTAB
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/kprobes.h>
#include <linux/mutex.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/page.h>
#include <asm/cacheflush.h>
#include <asm/apic.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <quaglia@dis.uniroma1.it>");
MODULE_DESCRIPTION("virtual to physical page mapping oracle");

#define MODNAME "VTPMO"

//stuff for sys cal table hacking
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,3,0)
    #include <asm/switch_to.h>
#else
    #include <asm/system.h>
#endif
#ifndef X86_CR0_WP
#define X86_CR0_WP 0x00010000
#endif


#define HACKED_ENTRIES 1

int restore[HACKED_ENTRIES] = {[0 ... (HACKED_ENTRIES-1)] -1};

/* please take the two values below from the system map */
unsigned long sys_call_table = 0xffffffff81800300;
unsigned long sys_ni_syscall = 0xffffffff8107e700;

//stuff for actual service operations
#define PML4(addr) (((long long)(addr) >> 39) & 0x1ff)
#define PDP(addr)  (((long long)(addr) >> 30) & 0x1ff)
#define PDE(addr)  (((long long)(addr) >> 21) & 0x1ff)
#define PTE(addr)  (((long long)(addr) >> 12) & 0x1ff)

#define NO_MAP (-1)
#define LARGE_PAGE_MAP (0)

#define AUDIT if(1)



int sys_vtpmo(unsigned long vaddr){
        void* target_address;
        void** pdp;
        void** pde;
        void** pte;
	void** pml4;
	int frame_number;
	unsigned long frame_addr;


        target_address = (void*)vaddr; 
        /* fixing the address to use for page table access */       

	AUDIT{
	printk("%s: \n------------------------------\n",MODNAME);
	printk("%s: vtpmo asked to tell the physical memory positioning of virtual address %p\n",MODNAME,(target_address));
	}

	pml4  = __va(read_cr3());
	printk("%s: PML4 traversing on entry %d\n",MODNAME,PML4(target_address));

	if(pml4[PML4(target_address)] == NULL){
		AUDIT
		printk("%s: PML4 region not mapped to physical memory\n",MODNAME);
		return NO_MAP;
        } 

	pdp = __va((ulong)pml4[PML4(target_address)] & 0xfffffffffffff000);           
	AUDIT
	printk("%s: PDP traversing on entry %d\n",MODNAME,PDP(target_address));


	if(pdp[PDP(target_address)] == NULL){ 
		AUDIT
		printk("%s: PDP region not mapped to physical memory\n",MODNAME);
		return NO_MAP;
	}

	pde = __va((ulong)pdp[PDP(target_address)] & 0x7ffffffffffff000);              
	AUDIT
	printk("%s: PDE traversing on entry %d\n",MODNAME,PDE(target_address));

	if(pde[PDE(target_address)] == NULL){ 
		AUDIT
		printk("%s: PDE region not mapped to physical memory\n",MODNAME);
		return NO_MAP;
	}

	if((ulong)pde[PDE(target_address)] & 0x080){ 
		AUDIT
		printk("%s: PDE region mapped to large page\n",MODNAME);
		return LARGE_PAGE_MAP;
	}

	pte = __va((ulong)pde[PDE(target_address)] & 0x7ffffffffffff000);              
	AUDIT
	printk("%s: PTE traversing on entry %d\n",MODNAME,PTE(target_address));

	if(pte[PTE(target_address)] == NULL){
		AUDIT
		printk("%s: PTE region (page) not mapped to physical memory\n",MODNAME);
		return NO_MAP;
	}

	frame_addr = (ulong)pte[PTE(target_address)] & 0x7ffffffffffff000; 
	printk("%s: frame physical addr is 0X%p\n",MODNAME,frame_addr);

	frame_number = frame_addr >> 12;

	return frame_number;
	
}


unsigned long new_sys_call_array[HACKED_ENTRIES] = {sys_vtpmo};

int init_module(void) {

        unsigned long * p = (unsigned long *) sys_call_table;
        int i,j;
        int ret;

        unsigned long cr0;

        printk("%s: initializing\n",MODNAME);
        j = -1;
        for (i=0; i<256; i++){
                if (p[i] == sys_ni_syscall){
                        printk("%s: table entry %d keeps address %p\n",MODNAME,i,(void*)p[i]);
                        j++;
                        restore[j] = i;
                        if (j == (HACKED_ENTRIES-1)) break;
                }
        }

        cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);
        for(i=0;i<HACKED_ENTRIES;i++){
                p[restore[i]] = (unsigned long)new_sys_call_array[i];
        }
        write_cr0(cr0);

        printk("%s: all new system-calls correctly installed on sys-call table \n",MODNAME);

        ret = 0;

        return ret;
}

void cleanup_module(void) {

        unsigned long * p = (unsigned long*) sys_call_table;
        unsigned long cr0;
        int i;
                
        printk("%s: shutting down\n",MODNAME);
        cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);
        for(i=0;i<HACKED_ENTRIES;i++){
                p[restore[i]] = sys_ni_syscall;
        }
        write_cr0(cr0);
        printk("%s: sys-call table restored to its original content\n",MODNAME);
        
}

