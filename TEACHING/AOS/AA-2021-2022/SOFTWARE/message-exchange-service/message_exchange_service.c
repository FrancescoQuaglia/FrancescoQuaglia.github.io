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
* @brief This is the source file for a didactical  Linux Kernel Module which implements
*	 a kernel level message exchange service with no intermediate buffering.
* @author Francesco Quaglia
*
* @date April 26, 2018
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

// This gives access to read_cr0() and write_cr0()
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,3,0)
    #include <asm/switch_to.h>
#else
    #include <asm/system.h>
#endif
#ifndef X86_CR0_WP
#define X86_CR0_WP 0x00010000
#endif


#define DEBUG if(1)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");
MODULE_DESCRIPTION("message exchange service via two system calls installed on available ni_sys_call entries of the syscall table");

#define MODNAME "MESSAGE EXCHANGE SERVICE"

int restore[2] = {[0 ... 1] -1};

unsigned long sys_call_table;// position of the syscall table - to be discovered at startup
unsigned long sys_ni_syscall_a;// position of the sys_ni_syscall code - to be discovred at startup

#define MAX_MSG_SIZE 4096
char  kernel_buff[MAX_MSG_SIZE];
size_t valid = 0;

static DEFINE_MUTEX(log_get_mutex);


asmlinkage long sys_log_message(char* mex, size_t size){
	
	int ret;

	printk("%s: sys_log_message has been called with params  %p - %d\n",MODNAME,mex,(int)size);

	if(size >= (MAX_MSG_SIZE -1)) goto bad_size;

	mutex_lock(&log_get_mutex);
       	ret = copy_from_user((char*)kernel_buff,(char*)mex,size);
	DEBUG
	printk("%s: sys_log_message copy from user returned %d\n",MODNAME,ret);
	kernel_buff[size-ret] = '\0';
	valid = size-ret;
	DEBUG
	printk("%s: kernel buffer updated content is: %s\n",MODNAME,kernel_buff);
	mutex_unlock(&log_get_mutex);

	return size-ret;

bad_size:

	return -1;
}

asmlinkage long sys_get_message(char* mex, size_t size){

	int ret;
	
	printk("%s: sys_get_message has been called with params  %p - %d\n",MODNAME,mex,(int)size);

	if(size > (MAX_MSG_SIZE)) goto bad_size;

	mutex_lock(&log_get_mutex);
	if (size > valid) size = valid; 
       	ret = copy_to_user((char*)mex,(char*)kernel_buff,size);
	DEBUG
	printk("%s: sys_get_message copy to user returned %d\n",MODNAME,ret);
	mutex_unlock(&log_get_mutex);
	return size - ret;

bad_size:

	return -1;
}

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

	unsigned long * p ; //= (unsigned long *) sys_call_table;
	int i,j;
	int ret;

	unsigned long cr0;

	unsigned long target;

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
			if (j == 1) break;
		}
	}

	cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);
	p[restore[0]] = (unsigned long)sys_log_message;
	p[restore[1]] = (unsigned long)sys_get_message;
	write_cr0(cr0);

	printk("%s: new system-call sys_log_message installed on sys-call table entry %d\n",MODNAME,restore[0]);
	printk("%s: new system-call sys_get_message installed on sys-call table entry %d\n",MODNAME,restore[1]);

	ret = 0;

	return ret;
}


void cleanup_module(void) {

	unsigned long * p = (unsigned long*) sys_call_table;
	unsigned long cr0;
        	
	printk("%s: shutting down\n",MODNAME);
	cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);
	p[restore[0]] = sys_ni_syscall_a;
	p[restore[1]] = sys_ni_syscall_a;
	write_cr0(cr0);
	printk("%s: sys-call table restored to its original content\n",MODNAME);
	
}

