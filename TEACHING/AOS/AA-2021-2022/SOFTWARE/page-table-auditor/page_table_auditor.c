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
* @file page_table_auditor.c 
* @brief This is the main source for the Linux Kernel Module which implements
*       a system call that can be used to audit via printk the current usage of page table entries 
*	for user/kernel memory mappings - this service is x86_64 specific in the curent implementation
*	if PTI is active, the audit will take place on the kernel page table
*	NOTE: it is a baseline that does not fully comply with atomicity requirements in concurrent 
*	kernel level memory map management!!!!
*
* @author Francesco Quaglia
*
* @date October 26, 2019
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

#define MODNAME "PAGE TABLE AUDIT"

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

int sys_page_table_audit(int);


unsigned long new_sys_call_array[] = {sys_page_table_audit};
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

#define ADDRESS_MASK 0xfffffffffffff000
#define PAGE_TABLE_ADDRESS phys_to_virt(_read_cr3() & ADDRESS_MASK)
#define PT_ADDRESS_MASK 0x7ffffffffffff000
#define VALID 0x1
#define MODE 0x100
#define LH_MAPPING 0x80
#define XD 0x8000000000000000

#define AUDIT if(0)

//x86-64 page table basic info
#define TABLE_ENTRIES 512

asmlinkage int sys_page_table_audit(int level){
        pud_t* pdp;
        pmd_t* pde;
        pte_t* pte;
	pgd_t *pml4;

	int i,j,k,t;

	printk("%s: ------------------------------\n",MODNAME);
	printk("%s: page table audit with level %d\n",MODNAME,level);

	if((level < 0) || (level > 3)){
		printk("%s: audit on invalide level\n",MODNAME);
		return -1;
	}

        pml4  = current->mm->pgd;
	printk("%s: pgd address is %p\n",MODNAME,pml4);

	pml4 = PAGE_TABLE_ADDRESS;
        printk("%s: cross check with CR3 is %p\n",MODNAME,pml4);

	for(i=0;i<TABLE_ENTRIES;i++){

                if(((ulong)(pml4[i].pgd)) & VALID){
                	printk("%s: valid pml4[%d] - mapping mode is %d - XD bit is %d\n",MODNAME,i,(((ulong)(pml4[i].pgd)) & MODE)>>2,(((ulong)(pml4[i].pgd)) & XD)>>63);
			if (level == 0) goto done_0;

			pdp = __va((ulong)(pml4[i].pgd) & PT_ADDRESS_MASK);           
			AUDIT
			printk("%s: pdp address is %p\n",MODNAME,pdp);

			for(j=0;j<TABLE_ENTRIES;j++){
				if((ulong)(pdp[j].pud) & VALID){
                			printk("%s: \tvalid pdp[%d] - mapping mode is %d - XD bit is %d - LH mapping is %d\n",MODNAME,j,(((ulong)(pdp[j].pud)) & MODE)>>2,(((ulong)(pdp[j].pud)) & XD)>>63,(((ulong)(pdp[j].pud)) & LH_MAPPING)>>7);

					if (level == 1 || (((ulong)(pdp[j].pud)) & LH_MAPPING)>>7) goto done_1;
					pde = __va((ulong)(pdp[j].pud) & PT_ADDRESS_MASK);              
					AUDIT
					printk("%s: \tpde address is %p\n",MODNAME,pde);

					for(k=0;k<TABLE_ENTRIES;k++){
						if((ulong)(pde[k].pmd) & VALID){
                					printk("%s: \t\tvalid pde[%d] - mapping mode is %d - XD bit is %d - LH mapping is %d\n",MODNAME,k,(((ulong)(pde[k].pmd)) & MODE)>>2,(((ulong)(pde[k].pmd)) & XD)>>63, (((ulong)(pde[k].pmd)) & LH_MAPPING)>>7);
	
							if (level == 2 || (((ulong)(pde[k].pmd)) & LH_MAPPING)>>7) goto done_2;
							pte = __va((ulong)(pde[j].pmd) & PT_ADDRESS_MASK);              
							AUDIT
							printk("%s: t\tpte address is %p\n",MODNAME,pte);

							for(t=0;t<TABLE_ENTRIES;t++){
								if((ulong)(pte[t].pte) & VALID){
                						printk("%s: \t\t\tvalid pte[%d] - mapping mode is %d - XD bit is %d\n",MODNAME,t,(((ulong)(pte[t].pte)) & MODE)>>2,(((ulong)(pte[t].pte)) & XD)>>63);
								}	
							}//level 3
done_2:;
					}
				}//level 2
done_1:;
				}
			}//level 1
done_0:;
		}
	}//level 0 

	return 0;
}

int init_module(void) {

        unsigned long cr0;
        int i;
        int ret;

        printk("%s: initializing - hacked entries %d\n",MODNAME,HACKED_ENTRIES);

        ret = get_entries(restore,HACKED_ENTRIES,&the_syscall_table,&the_ni_syscall);

        if (ret != HACKED_ENTRIES){
                printk("%s: could not hacked %d entires (just %d)\n",MODNAME,HACKED_ENTRIES,ret); 
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

