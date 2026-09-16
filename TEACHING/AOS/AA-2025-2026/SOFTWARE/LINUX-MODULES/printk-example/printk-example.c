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
* @date November 10, 2021
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
MODULE_DESCRIPTION("details on the usage of printk");

#define MODNAME "PRINTK EXAMPLE"

unsigned long the_syscall_table = 0x0;
module_param(the_syscall_table, ulong, 0660);


unsigned long the_ni_syscall;

unsigned long new_sys_call_array[] = {0x0};//please set to sys_vtpmo at startup
#define HACKED_ENTRIES (int)(sizeof(new_sys_call_array)/sizeof(unsigned long))
int restore[HACKED_ENTRIES] = {[0 ... (HACKED_ENTRIES-1)] -1};



#define AUDIT if(1)


#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
__SYSCALL_DEFINEx(1, _print, unsigned long, none){
#else
asmlinkage long sys_print(void){
#endif

	AUDIT{
		printk("%s: ------------------------------\n",MODNAME);
		printk("%s: asked to print e few messages\n",MODNAME);
	}


	printk(KERN_DEBUG "%s: message priority %s\n",MODNAME, KERN_DEBUG);
	printk(KERN_INFO "%s: message priority %s\n",MODNAME, KERN_INFO);
	printk(KERN_NOTICE "%s: message priority %s\n",MODNAME, KERN_NOTICE);
	printk(KERN_WARNING "%s: message priority %s\n",MODNAME, KERN_WARNING);
	printk(KERN_ERR "%s: message priority %s\n",MODNAME, KERN_ERR);
	printk(KERN_CRIT "%s: message priority %s\n",MODNAME, KERN_CRIT);
	printk(KERN_ALERT "%s: message priority %s\n",MODNAME, KERN_ALERT);
	printk(KERN_EMERG "%s: message priority %s\n",MODNAME, KERN_EMERG);

	return 0;
	
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
long sys_print = (unsigned long) __x64_sys_print;       
#else
#endif


int init_module(void) {

        int i;
        int ret;

        if (the_syscall_table == 0x0){
           printk("%s: cannot manage sys_call_table address set to 0x0\n",MODNAME);
           return -1;
        }

	AUDIT{
	   printk("%s: printk-example received sys_call_table address %px\n",MODNAME,(void*)the_syscall_table);
     	   printk("%s: initializing - hacked entries %d\n",MODNAME,HACKED_ENTRIES);
	}

	new_sys_call_array[0] = (unsigned long)sys_print;

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
