/* 
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
* @brief This is the main source for the Linux Kernel Module which adds a same system call
*	 accessible via both the fast and the slow system call paths. 
*        The addition exploits sys_ni_syscall pointers into the system call tables.
*	 This module is no longer compatible with kernel versions from 4.17
* @author Francesco Quaglia
*
* @date April 11, 2018
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

// This gives access to read_cr0() and write_cr0()
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,3,0)
    #include <asm/switch_to.h>
#else
    #include <asm/system.h>
#endif
#ifndef X86_CR0_WP
#define X86_CR0_WP 0x00010000
#endif


#define AUDIT if(1) //do not keep this active while doing performance tests with the user space code coupled with this module

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");

#define MODNAME "DUAL SYS-CALL TABLE BASIC HACKER"
#define HACKED_ENTRIES 1 //please modify the below variables to become arrays if more than 1 system call should be hacked in each table

int ia32_restore = -1;
int restore = -1;

/* please take the two values below from the system map */
unsigned long ia32_sys_call_table = 0xffffffff81c015c0; 
unsigned long sys_call_table = 0xffffffff81c00200;//this is for x86-64 
unsigned long compat_sys_ni_syscall = 0xffffffff810a81a0; 
unsigned long sys_ni_syscall = 0xffffffff810a81a0;//this is for x86-64


asmlinkage int sys_my_first_sys_call(void){
	AUDIT
	printk("%s: zero-params sys-call has been called\n",MODNAME);
	return 0;
}

asmlinkage int sys_my_second_sys_call(long a){
	AUDIT
	printk("%s: 1 param sys-call has been called (parame value %ld)\n",MODNAME,a);
	return 0;
}

unsigned long new_sys_call_array[HACKED_ENTRIES] = {(unsigned long)sys_my_first_sys_call,(unsigned long)sys_my_second_sys_call};

int init_module(void) {

	unsigned long * p = (unsigned long *) sys_call_table;
	int i;
	int ret;

	unsigned long cr0;

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 17, 0)
		 printk("%s: unsupported kernel version\n",MODNAME);
		 return -1;
	#else
	#endif

	printk("%s: initializing\n",MODNAME);
	for (i=0; i<256; i++){
		if (p[i] == sys_ni_syscall){
			printk("%s: table entry %d keeps address %p\n",MODNAME,i,(void*)p[i]);
			restore = i;
			break;
		}
	}

	p = (unsigned long *) ia32_sys_call_table;
	for (i=0; i<256; i++){
		if (p[i] == compat_sys_ni_syscall){
			printk("%s: ia32 table entry %d keeps address %p\n",MODNAME,i,(void*)p[i]);
			ia32_restore = i;
			break;
		}
	}

	if((restore == -1) || (ia32_restore == -1)){
		return -1;
	}

	cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);
	p = (void*)sys_call_table;
	for(i=0;i<HACKED_ENTRIES;i++){
		p[restore] = (unsigned long)new_sys_call_array[i];
	}
	p = (void*)ia32_sys_call_table;
	for(i=0;i<HACKED_ENTRIES;i++){
		p[ia32_restore] = (unsigned long)new_sys_call_array[i];
	}
	write_cr0(cr0);

	printk("%s: all new system-calls correctly installed on sys-call table \n",MODNAME);

	ret = 0;

	return ret;
}


void cleanup_module(void) {

	unsigned long * p = (unsigned long*) sys_call_table;
	unsigned long cr0;
        	
	printk("%s: shutting down\n",MODNAME);
	cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);

	p = (void *)sys_call_table;
	p[restore] = sys_ni_syscall;

	p = (void *)ia32_sys_call_table;
	p[ia32_restore] = compat_sys_ni_syscall;

	write_cr0(cr0);
	printk("%s: sys-call table restored to its original content\n",MODNAME);
	
}

