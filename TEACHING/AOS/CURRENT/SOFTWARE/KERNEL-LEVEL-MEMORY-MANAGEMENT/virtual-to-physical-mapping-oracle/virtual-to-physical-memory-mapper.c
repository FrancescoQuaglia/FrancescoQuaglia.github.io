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
* @date March 29, 2019
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
MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");
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


extern int get_entries(int *, int, unsigned long*, unsigned long* );

int sys_vtpmo(unsigned long);


unsigned long new_sys_call_array[] = {sys_vtpmo};
#define HACKED_ENTRIES (int)(sizeof(new_sys_call_array)/sizeof(unsigned long))
unsigned long the_syscall_table;
unsigned long the_ni_syscall;

int restore[HACKED_ENTRIES] = {[0 ... (HACKED_ENTRIES-1)] -1};

//auxiliary stuff
static inline unsigned long _read_cr3(void) {
          unsigned long val;
          asm volatile("mov %%cr3,%0":  "=r" (val) : );
          return val;
}


//stuff for actual service operations
#define ADDRESS_MASK 0xfffffffffffff000
#define PAGE_TABLE_ADDRESS phys_to_virt(_read_cr3() & ADDRESS_MASK)
#define PT_ADDRESS_MASK 0x7ffffffffffff000
#define VALID 0x1
#define MODE 0x100
#define LH_MAPPING 0x80

#define PML4(addr) (((long long)(addr) >> 39) & 0x1ff)
#define PDP(addr)  (((long long)(addr) >> 30) & 0x1ff)
#define PDE(addr)  (((long long)(addr) >> 21) & 0x1ff)
#define PTE(addr)  (((long long)(addr) >> 12) & 0x1ff)


#define NO_MAP (-1)

#define AUDIT if(1)



int sys_vtpmo(unsigned long vaddr){

        void* target_address;
        
	pud_t* pdp;
	pmd_t* pde;
	pte_t* pte;
	pgd_t *pml4;

	int frame_number;
	unsigned long frame_addr;


	target_address = (void*)vaddr; 
        /* fixing the address to use for page table access */       

	AUDIT{
	printk("%s: ------------------------------\n",MODNAME);
	printk("%s: vtpmo asked to tell the physical memory positioning (frame number) for virtual address %p\n",MODNAME,(target_address));
	}

	pml4  = PAGE_TABLE_ADDRESS;

	printk("%s: PML4 traversing on entry %d\n",MODNAME,PML4(target_address));

	if(((ulong)(pml4[PML4(target_address)].pgd)) & VALID){
	}
	else{
		AUDIT
		printk("%s: PML4 region not mapped to physical memory\n",MODNAME);
		return NO_MAP;
	} 

	pdp = __va((ulong)(pml4[PML4(target_address)].pgd) & PT_ADDRESS_MASK);           

    AUDIT
	printk("%s: PDP traversing on entry %d\n",MODNAME,PDP(target_address));

	if((ulong)(pdp[PDP(target_address)].pud) & VALID){
	}
	else{ 
		AUDIT
		printk("%s: PDP region not mapped to physical memory\n",MODNAME);
		return NO_MAP;
	}

	pde = __va((ulong)(pdp[PDP(target_address)].pud) & PT_ADDRESS_MASK);
	AUDIT
	printk("%s: PDE traversing on entry %d\n",MODNAME,PDE(target_address));

	if((ulong)(pde[PDE(target_address)].pmd) & VALID){
	}
	else{
		AUDIT
		printk("%s: PDE region not mapped to physical memory\n",MODNAME);
		return NO_MAP;
	}

	if((ulong)pde[PDE(target_address)].pmd & LH_MAPPING){ 
		AUDIT
		printk("%s: PDE region mapped to large page\n",MODNAME);
		frame_addr = (ulong)(pde[PDE(target_address)].pmd) & PT_ADDRESS_MASK; 
		printk("%s: frame physical addr is 0X%p\n",MODNAME,frame_addr);

		frame_number = frame_addr >> 12;

		return frame_number;
	}

	AUDIT
	printk("%s: PTE traversing on entry %d\n",MODNAME,PTE(target_address));

	pte = __va((ulong)(pde[PDE(target_address)].pmd) & PT_ADDRESS_MASK);           

 	if((ulong)(pte[PTE(target_address)].pte) & VALID){
	}
	else{
 		AUDIT
		printk("%s: PTE region (page) not mapped to physical memory\n",MODNAME);
		return NO_MAP;
	}

	frame_addr = (ulong)(pte[PTE(target_address)].pte) & PT_ADDRESS_MASK; 
	printk("%s: frame physical addr is 0X%p\n",MODNAME,frame_addr);

	frame_number = frame_addr >> 12;

	return frame_number;
	
}


int init_module(void) {

        unsigned long cr0;
        int i;
        int ret;

        printk("%s: initializing - hacked entries %d\n",MODNAME,HACKED_ENTRIES);

        ret = get_entries(restore,HACKED_ENTRIES,&the_syscall_table,&the_ni_syscall);

        if (ret != HACKED_ENTRIES){
                printk("%s: could not hack %d entires (just %d)\n",MODNAME,HACKED_ENTRIES,ret); 
                return -1;      
        }

        cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);

        for(i=0;i<HACKED_ENTRIES;i++){
                ((unsigned long *)the_syscall_table)[restore[i]] = (unsigned long)new_sys_call_array[i];
        }
        write_cr0(cr0);

        printk("%s: all new system-calls correctly installed on sys-call table\n",MODNAME);

        return 0;

}

void cleanup_module(void) {

        unsigned long cr0;
        int i;
                
        printk("%s: shutting down\n",MODNAME);
        cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);
        for(i=0;i<HACKED_ENTRIES;i++){
                ((unsigned long *)the_syscall_table)[restore[i]] = the_ni_syscall;
        }
        write_cr0(cr0);
        printk("%s: sys-call table restored to its original content\n",MODNAME);
        
}
