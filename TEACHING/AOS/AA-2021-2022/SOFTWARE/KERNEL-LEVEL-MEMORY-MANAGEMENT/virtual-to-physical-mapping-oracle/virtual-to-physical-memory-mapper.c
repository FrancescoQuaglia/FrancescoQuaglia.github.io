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
*
* @author Francesco Quaglia
*
* @date March 25, 2021
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
#include <asm/io.h>
#include <linux/syscalls.h>
#include "lib/include/scth.h"




MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");
MODULE_DESCRIPTION("virtual to physical page mapping oracle");

#define MODNAME "VTPMO"


unsigned long the_syscall_table = 0x0;
module_param(the_syscall_table, ulong, 0660);


unsigned long the_ni_syscall;

unsigned long new_sys_call_array[] = {0x0};//please set to sys_vtpmo at startup
#define HACKED_ENTRIES (int)(sizeof(new_sys_call_array)/sizeof(unsigned long))
int restore[HACKED_ENTRIES] = {[0 ... (HACKED_ENTRIES-1)] -1};


//auxiliary stuff
static unsigned long _read_cr3(void) {
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
#define IS_USER 0x4

#define PML4(addr) (((long long)(addr) >> 39) & 0x1ff)
#define PDP(addr)  (((long long)(addr) >> 30) & 0x1ff)
#define PDE(addr)  (((long long)(addr) >> 21) & 0x1ff)
#define PTE(addr)  (((long long)(addr) >> 12) & 0x1ff)


//#define NO_MAP (-1) //this is a baseline with no linkage to classical error code numbers
#define NO_MAP (-EFAULT)

#define AUDIT if(1)



#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
__SYSCALL_DEFINEx(1, _vtpmo, unsigned long, vaddr){
#else
asmlinkage long sys_vtpmo(unsigned long vaddr){
#endif
        void* target_address;
        
	pud_t* pdp;
	pmd_t* pde;
	pte_t* pte;
	pgd_t *pml4;

	long frame_number;
	unsigned long frame_addr;

	

	target_address = (void*)vaddr; 
        /* fixing the address to use for page table access */       

	AUDIT{
	printk("%s: ------------------------------\n",MODNAME);
	printk("%s: vtpmo asked to tell the physical memory positioning (frame number) for virtual address %p\n",MODNAME,(target_address));
	}

	pml4  = PAGE_TABLE_ADDRESS;

	spin_lock(&current->mm->page_table_lock);//still not using split page table locks

	printk("%s: PML4 traversing on entry %lld\n",MODNAME,PML4(target_address));

	if(((ulong)(pml4[PML4(target_address)].pgd)) & VALID){
	}
	else{
		AUDIT
		printk("%s: PML4 region not mapped to physical memory\n",MODNAME);
		spin_unlock(&current->mm->page_table_lock);
		return NO_MAP;
	} 

	pdp = __va((ulong)(pml4[PML4(target_address)].pgd) & PT_ADDRESS_MASK);           

	AUDIT
	printk("%s: PDP traversing on entry %lld\n",MODNAME,PDP(target_address));

	if((ulong)(pdp[PDP(target_address)].pud) & VALID){
	}
	else{ 
		AUDIT
		printk("%s: PDP region not mapped to physical memory\n",MODNAME);
		spin_unlock(&current->mm->page_table_lock);
		return NO_MAP;
	}

	pde = __va((ulong)(pdp[PDP(target_address)].pud) & PT_ADDRESS_MASK);
	AUDIT
	printk("%s: PDE traversing on entry %lld\n",MODNAME,PDE(target_address));

	if(((ulong)(pde[PDE(target_address)].pmd) & VALID) && ((ulong)(pde[PDE(target_address)].pmd) & IS_USER)){
	}
	else{
		AUDIT
		printk("%s: PDE region not mapped to physical memory\n",MODNAME);
		spin_unlock(&current->mm->page_table_lock);
		return NO_MAP;
	}

	if((ulong)pde[PDE(target_address)].pmd & LH_MAPPING){ 
		AUDIT
		printk("%s: PDE region mapped to large page\n",MODNAME);
		frame_addr = (ulong)(pde[PDE(target_address)].pmd) & PT_ADDRESS_MASK; 
		printk("%s: frame physical addr is 0X%p\n",MODNAME,(void*)frame_addr);

		frame_number = frame_addr >> 12;

		spin_unlock(&current->mm->page_table_lock);

		return frame_number;

	}

	AUDIT
	printk("%s: PTE traversing on entry %lld\n",MODNAME,PTE(target_address));

	pte = __va((ulong)(pde[PDE(target_address)].pmd) & PT_ADDRESS_MASK);           

 	if(((ulong)(pte[PTE(target_address)].pte) & VALID) && ((ulong)(pte[PTE(target_address)].pte) & IS_USER)){
	}
	else{
 		AUDIT
		printk("%s: PTE region (page) not mapped to physical memory\n",MODNAME);
		spin_unlock(&current->mm->page_table_lock);
		return NO_MAP;
	}



	frame_addr = (ulong)(pte[PTE(target_address)].pte) & PT_ADDRESS_MASK; 
	spin_unlock(&current->mm->page_table_lock);

	printk("%s: frame physical addr is 0X%p\n",MODNAME,(void*)frame_addr);

	frame_number = frame_addr >> 12;
	
	return frame_number;
	
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
long sys_vtpmo = (unsigned long) __x64_sys_vtpmo;       
#else
#endif


int init_module(void) {

        int i;
        int ret;

	AUDIT{
	   printk("%s: vtpmo received sys_call_table address %px\n",MODNAME,(void*)the_syscall_table);
     	   printk("%s: initializing - hacked entries %d\n",MODNAME,HACKED_ENTRIES);
	}

	new_sys_call_array[0] = (unsigned long)sys_vtpmo;

        ret = get_entries(restore,HACKED_ENTRIES,(unsigned long*)the_syscall_table,&the_ni_syscall);

        if (ret != HACKED_ENTRIES){
                printk("%s: could not hack %d entries (just %d)\n",MODNAME,HACKED_ENTRIES,ret); 
                return -1;      
        }

	unprotect_memory();

        for(i=0;i<HACKED_ENTRIES;i++){
                ((unsigned long *)the_syscall_table)[restore[i]] = (unsigned long)new_sys_call_array[i];
        }

	protect_memory();

        printk("%s: all new system-calls correctly installed on sys-call table\n",MODNAME);

        return 0;

}

void cleanup_module(void) {

        int i;
                
        printk("%s: shutting down\n",MODNAME);

	unprotect_memory();
        for(i=0;i<HACKED_ENTRIES;i++){
                ((unsigned long *)the_syscall_table)[restore[i]] = the_ni_syscall;
        }
	protect_memory();
        printk("%s: sys-call table restored to its original content\n",MODNAME);
        
}
