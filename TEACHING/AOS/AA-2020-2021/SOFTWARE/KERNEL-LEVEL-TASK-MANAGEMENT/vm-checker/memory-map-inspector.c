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
* @brief This is the main source for the Linux Kernel Module which implements
*       a system call that can be used to query the kernel for current protection on vm_areas
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
#include <linux/syscalls.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");
MODULE_DESCRIPTION("memory map inspector");

#define MODNAME "MMINSP"

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

unsigned long sys_call_table;// position of the syscall table - to be discovered at startup
unsigned long sys_ni_syscall_a;// position of the sys_ni_syscall code - to be discovred at startup


#define AUDIT if(1)


int sys_mm_inspect(unsigned long vaddr, int mode){

	struct vm_area_struct *map;
	pgprot_t prot;
	int ret = 0;
	unsigned long mask;



	AUDIT{
	printk("%s: \n------------------------------\n",MODNAME);
	printk("%s: mm inspector asked to tell memory at virtual address %lu is accessible in mode %d\n",MODNAME,(void*)vaddr,mode);
	}


	down_read(&current->mm->mmap_sem);//this is just a traversal with reads
	map = current->mm->mmap;

	while( map!=NULL ){

		printk("%s: traversing entry for zone [%lu,%lu]\n",MODNAME,map->vm_start,map->vm_end);

		if((map->vm_start <= vaddr) && ((map->vm_end-1)>= vaddr)){

			printk("%s: target address found in entry for zone [%d,%d]\n",MODNAME,map->vm_start,map->vm_end);

			prot = map->vm_page_prot;
			mask = (unsigned long)pgprot_val(prot);

			switch (mode) {

				case 0:  
					printk("%s: read only query\n",MODNAME);
					if( !(mask & 0x2)) ret = 1;
					break;

				case 1: 
					printk("%s: read/write query\n",MODNAME);
					if( !(mask & 0x2)) ret = 1;
					break;

				case 2: 
					printk("%s: exec query\n",MODNAME);
					if( !(mask & 0x8000000000000000) ) ret = 1;
					break;

			}
				
			up_read(&current->mm->mmap_sem);//it was just a traversal with reads

			return ret;
		}

		map = map->vm_next;	

	}	

	up_read(&current->mm->mmap_sem);//it was just a traversal with reads
	return -1;
}


unsigned long new_sys_call_array[HACKED_ENTRIES] = {sys_mm_inspect};

unsigned long *get_syscall_table(void)
{       
        unsigned long *syscall_table;
        unsigned long int i;
        
        for (i = (unsigned long int)sys_close; i < ULONG_MAX; i += sizeof(void *)) {
                syscall_table = (unsigned long *)i;
                
                if (syscall_table[__NR_close] == (unsigned long)sys_close)
                        return syscall_table;
        }
        return NULL;
}


int init_module(void) {

        unsigned long * p; // = (unsigned long *) sys_call_table;
        int i,j;
        int ret;
	unsigned long target;

        unsigned long cr0;

        printk("%s: initializing\n",MODNAME);

	        p = sys_call_table = get_syscall_table();

        if(p == NULL){

                printk("%s: could not locate syscall table position\n",MODNAME);
                return -1;

        }

    	for(i=0;i<256;i++){
                for(j=i+1;j<256;j++){
                        if(p[i] == p[j]){
                                target = p[i];
                                goto found;
                        }
                }

        }

        return -1;

found:

        sys_ni_syscall_a = target;

	   j = -1;
        for (i=0; i<256; i++){
                if (p[i] == sys_ni_syscall_a){
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
                p[restore[i]] = sys_ni_syscall_a;
        }
        write_cr0(cr0);
        printk("%s: sys-call table restored to its original content\n",MODNAME);
        
}

