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
*       a system call that can be used to audit via printk the current usage of page table entries 
*	for user/kernel memory mappings - this service is x86_64 specific in the curent implementation
*	NOTE: it is a baseline that does not fully comply with atomicity requirements in concurrent 
*	kernel level memory map management!!!!
*
* @author Francesco Quaglia
*
* @date November 8, 2018
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

#define MODNAME "MAPPINGS AUDIT"

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


#define AUDIT if(1)
#define LEVEL3_AUDIT if(0)

#define TABLE_ENTRIES 512

int sys_mapping_audit(unsigned long vaddr){
        void** pdp;
        void** pde;
        void** pte;
	void** pml4;

	int i,j,k,t;

	AUDIT{
	printk("%s: \n------------------------------\n",MODNAME);
	printk("%s: mappings audit \n",MODNAME);
	}

	pml4  = __va(read_cr3());
	printk("%s: pml4 address is %p\n",MODNAME,pml4);

	for(i=0;i<TABLE_ENTRIES;i++){
		if(((ulong)pml4[i]) & 0x1){
			printk("%s: valid pml4[%d] - mapping mode is %d\n",MODNAME,i,(((ulong)pml4[i]) & 0x4)>>2);
			pdp = __va((ulong)pml4[i] & 0x7ffffffffffff000);           
			printk("%s: pdp address is %p\n",MODNAME,pdp);

			for(j=0;j<TABLE_ENTRIES;j++){
				if(((ulong)pdp[j]) & 0x1){
					printk("%s: \tvalid pdp[%d] - mapping mode is %d\n",MODNAME,j,(((ulong)pdp[j]) & 0x4)>>2);
					pde = __va((ulong)pdp[j] & 0x7ffffffffffff000);              
					printk("%s: \tpde address is %p\n",MODNAME,pde);

					for(k=0;k<TABLE_ENTRIES;k++){
						if(((ulong)pde[k]) & 0x1){
							LEVEL3_AUDIT
							printk("%s: \t\tvalid pde[%d] - mapping mode is %d\n",MODNAME,k,(((ulong)pde[k]) & 0x4)>>2);

							for(t=0;t<TABLE_ENTRIES;t++){
							//please complete with LEVEL4 info
							}
						}
					}
				}
			}
		}
	}

	return 0;
	
}


unsigned long new_sys_call_array[HACKED_ENTRIES] = {sys_mapping_audit};

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

