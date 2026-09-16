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
* @brief This is the source file for a didactical Linux Kernel Module which implements
*	 a kernel level message exchange service with intermediate buffering.
* @author Francesco Quaglia
*
* @date October 4, 2021
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
#include "lib/include/scth.h"


#define AUDIT if(1)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");
MODULE_DESCRIPTION("message exchange service via two system calls installed on available ni_sys_call entries of the syscall table");

#define MODNAME "MESSAGE EXCHANGE SERVICE"


unsigned long the_syscall_table = 0x0;
module_param(the_syscall_table, ulong, 0660);

unsigned long the_ni_syscall;


#define MAX_MSG_SIZE 4096
char  kernel_buff[MAX_MSG_SIZE];
size_t valid = 0;

static DEFINE_MUTEX(log_get_mutex);

unsigned long new_sys_call_array[] = {0x0,0x0};//please set to your API 
#define HACKED_ENTRIES (int)(sizeof(new_sys_call_array)/sizeof(unsigned long))
int restore[HACKED_ENTRIES] = {[0 ... (HACKED_ENTRIES-1)] -1};


#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
__SYSCALL_DEFINEx(2, _log_message, char*, mex, size_t, size){
#else
asmlinkage long sys_log_message(char* mex, size_t size){
#endif
        
        unsigned long ret;
        void* addr;

        AUDIT
        printk("%s: sys_log_message has been called with params  %p - %d\n",MODNAME,mex,(int)size);

        if(size >= (MAX_MSG_SIZE -1)) goto bad_size;//leave 1 byte for string terminator

        addr = (void*)get_zeroed_page(GFP_KERNEL);

	if (addr == NULL) return -ENOMEM;

        ret = copy_from_user((char*)addr,(char*)mex,size);//returns the number of bytes NOT copied

        mutex_lock(&log_get_mutex);
        memcpy((char*)kernel_buff,(char*)addr,size-ret);
        kernel_buff[size - ret] = '\0';

        AUDIT
        printk("%s: kernel buffer updated content is: %s\n",MODNAME,kernel_buff);

        valid = size - ret;
        mutex_unlock(&log_get_mutex);

        free_pages((unsigned long)addr,0);

        return size - ret;

bad_size:

        return -EINVAL;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
__SYSCALL_DEFINEx(2, _get_message, char*, mex, size_t, size){
#else
asmlinkage long sys_get_message(char* mex, size_t size){
#endif

        unsigned long ret = 0;
        void* addr;
        
        AUDIT
        printk("%s: sys_get_message has been called with params  %p - %d\n",MODNAME,mex,(int)size);

        if(size > MAX_MSG_SIZE) goto bad_size;

        addr = (void*)get_zeroed_page(GFP_KERNEL);

	if (addr == NULL) return -ENOMEM;

        mutex_lock(&log_get_mutex);
        if (size > valid) size = valid; 
        memcpy((char*)addr,(char*)kernel_buff,size);
        mutex_unlock(&log_get_mutex);

        ret = copy_to_user((char*)mex,(char*)addr,size);
        free_pages((unsigned long)addr,0);
        
        AUDIT
        printk("%s: sys_get_message copy to user returned %d\n",MODNAME,(int)ret);
        return size - ret;

bad_size:

        return -EINVAL;
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
long sys_log_message = (unsigned long) __x64_sys_log_message;
long sys_get_message = (unsigned long) __x64_sys_get_message;
#else
#endif


int init_module(void) {

	int i;
	int ret;


        printk("%s: initializing\n",MODNAME);

	if (the_syscall_table == 0x0){
           printk("%s: cannot manage sys_call_table address set to 0x0\n",MODNAME);
           return -1;
        }
        
	new_sys_call_array[0] = (unsigned long)sys_log_message;
	new_sys_call_array[1] = (unsigned long)sys_get_message;

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

