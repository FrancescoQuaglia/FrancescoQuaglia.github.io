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
* @brief This is the main source for the Linux Kernel Module hacks the system 
	  call table to insert new system calls
* @author Francesco Quaglia
*
* @date March 27, 2017
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


#define DEBUG if(0)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <quaglia@dis.uniroma1.it>");
MODULE_DESCRIPTION("nesting of new system calls on the first entry originally reserved for sys_ni_syscall");
MODULE_DESCRIPTION("the new system calls simply printk a string");

#define MODNAME "SYS-CALL TABLE BASIC HACKER"

/* please take the two values below from the system map */
unsigned long sys_call_table = 0xffffffff81c00200; 
unsigned long sys_ni_syscall = 0xffffffff810a81a0;


asmlinkage int sys_my_first_sys_call(void){
	printk("%s: zero-params sys-call has been called\n",MODNAME);
	return 0;
}

asmlinkage int sys_my_second_sys_call(int a){
	printk("%s: 1-param sys-call has been called (with param %d)\n",MODNAME,a);
	return 0;
}

unsigned long new_sys_call_array[] = {(unsigned long)sys_my_first_sys_call,(unsigned long)sys_my_second_sys_call};
#define HACKED_ENTRIES (sizeof(new_sys_call_array)>>3) 

int restore[HACKED_ENTRIES] = {[0 ... (HACKED_ENTRIES-1)] -1};

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

	if(j != (HACKED_ENTRIES-1)){//we have a failure - no room found in the syst-call table for the new system calls
		return -1;
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
		if (restore[i] != -1){
			p[restore[i]] = sys_ni_syscall;
		}
	}
	write_cr0(cr0);
	printk("%s: sys-call table restored to its original content\n",MODNAME);
	
}

