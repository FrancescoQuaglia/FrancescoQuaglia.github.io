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
* @file example-usage.c 
* @brief This is the main source for the Linux Kernel Module which exploits the scth.c module
* 	which implements the automatic discovery of the sys_call_table in the Linux kernel image
* 	and determines if a given number of entries are free (they point to sys_ni_syscall) for installing
* 	alternative syscalls. In the positive case the indexes of the free entries are returned together with 
* 	the address of the sys_call_table and the address of sys_ni_syscall. The latter two can be usefull
* 	for sys_call_table resotre operations
*
* @author Francesco Quaglia
*
* @date  October 20, 2019
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
MODULE_AUTHOR("Francesco Quaglia <quaglia@dis.uniroma1.it>");
MODULE_DESCRIPTION("example usage of scth");

#define MODNAME "SCTH EXPLOITER"

//stuff for sys call table updating 
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,3,0)
    #include <asm/switch_to.h>
#else
    #include <asm/system.h>
#endif
#ifndef X86_CR0_WP
#define X86_CR0_WP 0x00010000
#endif

extern int get_entries(int *, int, unsigned long*, unsigned long* );

int sys_new_syscall(void);


unsigned long new_sys_call_array[] = {(unsigned long)sys_new_syscall,(unsigned long)sys_new_syscall};
#define HACKED_ENTRIES (int)(sizeof(new_sys_call_array)/sizeof(unsigned long))
unsigned long the_syscall_table;//we will discover where it is at runtime
unsigned long the_ni_syscall;//we will discover where it is at runtime

int restore[HACKED_ENTRIES] = {[0 ... (HACKED_ENTRIES-1)] -1};//to restore the initial content of the sys_call_table


#define AUDIT if(1)




asmlinkage int sys_new_syscall(void){

	AUDIT{
	printk("%s: ------------------------------\n",MODNAME);
	printk("%s: new syscall called \n",MODNAME);
	}
	return 0;

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

