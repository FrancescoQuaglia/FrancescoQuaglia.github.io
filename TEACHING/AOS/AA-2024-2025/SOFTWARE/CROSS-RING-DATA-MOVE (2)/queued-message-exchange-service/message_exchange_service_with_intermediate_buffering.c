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
*	 a kernel level message exchange service with intermediate buffering
* @author Francesco Quaglia
*
* @date November 11, 2022
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
MODULE_DESCRIPTION("message exchange service layered on cached allocation via two system calls installed on available ni_sys_call entries of the syscall table");

#define MODNAME "MESSAGE EXCHANGE SERVICE"


unsigned long the_syscall_table = 0x0;
module_param(the_syscall_table, ulong, 0660);

unsigned long the_ni_syscall;


unsigned long new_sys_call_array[] = {0x0,0x0};//please set to your API 
#define HACKED_ENTRIES (int)(sizeof(new_sys_call_array)/sizeof(unsigned long))
int restore[HACKED_ENTRIES] = {[0 ... (HACKED_ENTRIES-1)] -1};


//stuff for the queue of messages
#define MAX_MESSAGE_SIZE 120

typedef struct _elem{
	char msg[MAX_MESSAGE_SIZE];
	int msg_size;
        struct _elem * next;
        struct _elem * prev;
} elem;

elem head = {"f",0,NULL,NULL};
elem tail = {"f",0,NULL,NULL};
spinlock_t queue_lock;

struct kmem_cache *the_cache;

void setup_area(void* buffer){
	//here you can do whathever you want
	//printk("%s: setting up message buffer att address %p\n",MODNAME,buffer);
	//memset(buffer,'f', sizeof(elem));
}


//the system calls
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
__SYSCALL_DEFINEx(2, _log_message, char*, mex, size_t, size){
#else
asmlinkage long sys_log_message(char* mex, size_t size){
#endif
        
        unsigned long ret;
	elem *addr;
	elem *aux;
	char msg_text[MAX_MESSAGE_SIZE];

        AUDIT
        printk("%s: sys_log_message has been called with params  %p - %d\n",MODNAME,mex,(int)size);

        if(size < 0) return -EINVAL;

        if(size > MAX_MESSAGE_SIZE) return -EINVAL;

	addr = kmem_cache_alloc(the_cache, GFP_USER);

	if (addr == NULL) return -ENOMEM;
	
	//we make an intermediate copy on the stack area to avoid sgfaults caused
	//by user/kernel data move on memcaches not managed by kmalloc
        ret = copy_from_user((char*)msg_text,(char*)mex,size);//returns the number of bytes NOT copied
	addr->msg_size = size - ret;

	//final copy of data onto the memcache
	memcpy((char*)addr->msg,msg_text,addr->msg_size);

        spin_lock(&queue_lock);

	aux = &tail;
        if(aux->prev == NULL){
                spin_unlock(&queue_lock);
                printk("%s: malformed message queue - service damaged\n",MODNAME);
                return -1;
        }

	addr->prev = aux->prev;
	aux->prev->next = addr;
	addr->next = aux;
	aux->prev = addr;

        spin_unlock(&queue_lock);

        return size - ret;

}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
__SYSCALL_DEFINEx(2, _get_message, char*, mex, size_t, size){
#else
asmlinkage long sys_get_message(char* mex, size_t size){
#endif

        unsigned long ret;
        elem* addr;
        elem* aux;
	char msg_text[MAX_MESSAGE_SIZE];
        
        AUDIT
        printk("%s: sys_get_message has been called with params  %p - %d\n",MODNAME,mex,(int)size);

        if(size < 0) return -EINVAL;

        spin_lock(&queue_lock);

	aux = &head;
        if(aux->next == NULL){
                spin_unlock(&queue_lock);
                printk("%s: malformed message queue - service damaged\n",MODNAME);
                return -1;
        }

        if(aux->next == &tail){
                spin_unlock(&queue_lock);
                printk("%s: no message available\n",MODNAME);
                return 0;
        }

	addr = aux->next;
	addr->next->prev = aux;
	aux->next = addr->next;

        spin_unlock(&queue_lock);

	if (addr->msg_size > size) { addr->msg_size = size; } 
	//we make an intermediate copy to the stack area to avoid sgfaults caused
	//by user/kernel data move on memcaches not managed by kmalloc
	memcpy(msg_text,addr->msg,addr->msg_size);

        ret = copy_to_user((char*)mex,(char*)msg_text, addr->msg_size);

	kmem_cache_free(the_cache,addr);
        
        AUDIT
        printk("%s: sys_get_message copy to user returned %d\n",MODNAME,(int)(addr->msg_size-ret));

        return addr->msg_size - ret;

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

	the_cache = kmem_cache_create ("messaging-system", sizeof(elem), 0, SLAB_POISON, setup_area);

        if (the_cache == NULL){
                printk("%s: could not setup the service memcache\n",MODNAME);
                return -ENOMEM;
        }

	spin_lock_init(&queue_lock);//setup of the queue lock

	head.next = &tail;// setup initial double linked list
        tail.prev = &head;
        
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

	kmem_cache_destroy(the_cache);

        printk("%s: messaging cache destroy attempted\n",MODNAME);

}

