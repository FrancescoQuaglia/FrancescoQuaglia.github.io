/**		      Copyright (C) 2014-2015 HPDCS Group
*		       http://www.dis.uniroma1.it/~hpdcs
* 
* This is free software; 
* You can redistribute it and/or modify this file under the
* terms of the GNU General Public License as published by the Free Software
* Foundation; either version 3 of the License, or (at your option) any later
* version.
* 
* This file is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
* A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License along with
* this file; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
* 
* @author Francesco Quaglia
*
* @date April 29, 2015
*/
#define EXPORT_SYMTAB
#include <linux/module.h>
#include <linux/moduleparam.h>
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
#include "log.h"

// This gives access to read_cr0() and write_cr0()
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,3,0)
    #include <asm/switch_to.h>
#else
    #include <asm/system.h>
#endif
#ifndef X86_CR0_WP
#define X86_CR0_WP 0x00010000
#endif


#define DEBUG if(0)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <quaglia@dis.uniroma1.it>");
MODULE_DESCRIPTION("nesting of new system calls on the first entry originally reserved for sys_ni_syscall");
MODULE_DESCRIPTION("the new system calls implement a page-fault monitoring service");
MODULE_DESCRIPTION("the module needs to be couple with the intercept module, which istalls a custo PF handler onto the IDT");

#define MODNAME "PF MONITOR"


int restore[2] = {[0 ... 1] -1};

/* please take the two values below from the system map */
unsigned long sys_call_table = 0xffffffff81800300;
unsigned long sys_ni_syscall = 0xffffffff8107e700;


asmlinkage long sys_monitor_reset(void){
	
	pf_log * p;

	printk("%s: sys_monitor_reset has been called - current is %d\n",MODNAME,current->pid);

	p = (void*)current->stack + sizeof(struct thread_info);

	p->read = 0;
	p->write = 0;

	return 0;
}

asmlinkage long sys_get_pf(pf_log * buff){

	pf_log * p;
	int ret;
	
	printk("%s: sys_get_pf has been called - current is %d\n",MODNAME,current->pid);

	p = (void*)current->stack + sizeof(struct thread_info);

	printk("%s: pf info is; read %d - write %d \n",MODNAME,p->read,p->write);

       	ret = copy_to_user((char*)buff,(char*)p,sizeof(pf_log));

	return 0;

}

int init_module(void) {

	unsigned long * p = (unsigned long *) sys_call_table;
	int i,j;

	unsigned long cr0;

	printk("%s: initializing\n",MODNAME);

	j = -1;
	for (i=0; i<256; i++){
		if (p[i] == sys_ni_syscall){
			printk("%s: table entry %d keeps address %p\n",MODNAME,i,(void*)p[i]);
			j++;
			restore[j] = i;
			if (j == 1) break;
		}
	}

	cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);
	p[restore[0]] = (unsigned long)sys_monitor_reset;
	p[restore[1]] = (unsigned long)sys_get_pf;
	write_cr0(cr0);

	printk("%s: new system-call sys_monitor_reset installed on sys-call table entry %d\n",MODNAME,restore[0]);
	printk("%s: new system-call sys_get_pf installed on sys-call table entry %d\n",MODNAME,restore[1]);

	return  0;

}


void cleanup_module(void) {

	unsigned long * p = (unsigned long*) sys_call_table;
	unsigned long cr0;
        	
	printk("%s: shutting down\n",MODNAME);
	cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);
	p[restore[0]] = sys_ni_syscall;
	p[restore[1]] = sys_ni_syscall;
	write_cr0(cr0);
	printk("%s: sys-call table restored to its original content\n",MODNAME);
	
}

