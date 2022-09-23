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
* @file mod.c 
* @brief This is a source file for the Linux Kernel Module which implements
*       a trap-based coxtext switch among new threads of a new kind  
*       the module does not account for vectorized registers in order to save/restore
*       the context of a thread
*
* @author Francesco Quaglia
*
* @date November 27, 2021
*/




#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kprobes.h>

#include "lib/include/scth.h"
#include "control.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");
MODULE_DESCRIPTION("trap-based context switch");

extern void my_handler(void);

extern elem head, tail;
extern long sys_thread_manager_reset(void);
extern long sys_new_thread(unsigned int, unsigned int);

unsigned long the_syscall_table = 0x0;
module_param(the_syscall_table, ulong, 0660);


unsigned long the_ni_syscall;

unsigned long new_sys_call_array[] = {0x0,0x0};//please set to sys_got_sleep and sys_awake at startup
#define HACKED_ENTRIES (int)(sizeof(new_sys_call_array)/sizeof(unsigned long))
int restore[HACKED_ENTRIES] = {[0 ... (HACKED_ENTRIES-1)] -1};


static __init int my_irq_init(void) {

	int err = 0;
	int i;
	int ret;

	printk("%s: module init\n",MODNAME);

        head.next = &tail;// setup initial double linked list
        tail.prev = &head;

	new_sys_call_array[0] = (unsigned long)sys_thread_manager_reset;
        new_sys_call_array[1] = (unsigned long)sys_new_thread;

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


	err = setup_my_irq();
	if (err) {
		unprotect_memory();
        	for(i=0;i<HACKED_ENTRIES;i++){
               		 ((unsigned long *)the_syscall_table)[restore[i]] = the_ni_syscall;
        	}
        	protect_memory();
	}

	return err;

}

void __exit my_irq_exit(void) {

	int i;

	cleanup_my_irq();

	unprotect_memory();
        for(i=0;i<HACKED_ENTRIES;i++){
                ((unsigned long *)the_syscall_table)[restore[i]] = the_ni_syscall;
        }
        protect_memory();
        printk("%s: sys-call table restored to its original content\n",MODNAME);

	pr_info("%s: module exit\n",MODNAME);

}

module_init(my_irq_init);
module_exit(my_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia");
