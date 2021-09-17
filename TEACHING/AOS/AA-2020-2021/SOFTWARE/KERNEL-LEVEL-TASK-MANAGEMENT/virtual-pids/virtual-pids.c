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
*	 queries on PID namespaces
*
* @author Francesco Quaglia
*
* @date March 27, 2018
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
MODULE_DESCRIPTION("pid inspector");

#define MODNAME "PID Query"

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


char *pr = "REAL";

char *pv = "VIRTUAL";


unsigned int get_pid_service(int type){

 	char *p;
	unsigned int the_pid;

	p = type? pv: pr; 

	printk("%s: get pid service asked to deliver PID type %s\n",MODNAME,p); 
	
	if(type){
		the_pid = current->nsproxy->pid_ns_for_children->last_pid;//moved to pid_allocated in latest kernel versions 
	}
	else{
		the_pid = current->pid;
	}
	
	printk("%s: address checking - current is %p - find task by (v)PID is %p\n",MODNAME,current,pid_task(find_vpid(the_pid),PIDTYPE_PID)); 
	
	return the_pid;

}


unsigned long new_sys_call_array[HACKED_ENTRIES] = {get_pid_service};

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

