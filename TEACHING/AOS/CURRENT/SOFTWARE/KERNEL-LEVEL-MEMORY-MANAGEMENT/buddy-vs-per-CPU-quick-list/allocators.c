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
#include <linux/syscalls.h>
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
MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");
MODULE_DESCRIPTION("addition of two system calls supporting a performance test for comparing buddy vs per-CPU quicklis allocation - when not already supported by the buddy allocator");
MODULE_DESCRIPTION("this module is not suited for regular user/kernel mode interactions, it is only suited for performance comparisons");
MODULE_DESCRIPTION("further, although N CPUs can be configured, just one at a time can be safely used for the tests");

#define MODNAME "MEMORY ALLOCATION TEST"
#define HACKED_ENTRIES 2

int restore[HACKED_ENTRIES] = {[0 ... (HACKED_ENTRIES-1)] -1};

unsigned long sys_ni_syscall_a;


#define LOCAL

#ifdef LOCAL
#define NR_CPU 2
void* lists[NR_CPU];
DEFINE_PER_CPU(int, me);
#endif


asmlinkage void* get_buffer(void){
	void* the_addr = NULL;

#ifdef LOCAL
	int *x;
	int cpu_id;
	static int set = 0;

	if(set == 0){
		x = this_cpu_ptr(&me);
                *x = smp_processor_id();
                set = 1;
	}
	cpu_id = this_cpu_read(me);
	the_addr = lists[cpu_id];
	lists[cpu_id] = *((void**)the_addr);
	
#else
	the_addr = __get_free_pages(GFP_KERNEL,0);
#endif
	DEBUG
	printk("%s: get_buffer called - allocation returned address %p\n",MODNAME,the_addr);
	if(the_addr == NULL){
		return NULL;
	}	

	return the_addr;
}

asmlinkage int release_buffer(void* addr){

	DEBUG
	printk("%s: release_buffer called with adress %p)\n",MODNAME,addr);
	
	if(addr == NULL) return -1;

#ifdef LOCAL
	int *x;
	int cpu_id;
	static int set = 0;

	if(set == 0){
		x = this_cpu_ptr(&me);
                *x = smp_processor_id();
                set = 1;
	}
	cpu_id = this_cpu_read(me);
	
	*((void**)addr) = lists[cpu_id];
	lists[cpu_id] = addr;
#else
	free_pages(addr,0);
#endif
	return 0;
}

unsigned long new_sys_call_array[HACKED_ENTRIES] = {get_buffer,release_buffer};


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

	unsigned long * p;// = (unsigned long *) sys_call_table;
	int i,j;
	int ret;

	unsigned long cr0;
	
	unsigned long target = 0x0;

	printk("%s: initializing\n",MODNAME);
	

	p = get_syscall_table();

	if(p == NULL){

		printk("%s: coul not locate syscall table position\n",MODNAME);
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

	
	printk("%s: syscall table scan returned no sys_ni_syscall address\n",MODNAME);

	ret -EPERM;

found:
	sys_ni_syscall_a = target;

	printk("%s: found target is %p\n",MODNAME,(void*)target);


	j = -1;
	for (i=0; i<256; i++){
		if (p[i] == sys_ni_syscall_a){
			printk("%s: table entry %d keeps address %p\n",MODNAME,i,(void*)p[i]);
			j++;
			restore[j] = i;
			if (j == (HACKED_ENTRIES-1)) break;
		}
	}

	if(j != (HACKED_ENTRIES-1)){//we have a failure - no room found in the syst-call table for the new system calls
		return -EPERM;
	}

#ifdef LOCAL
	for(i=0;i<NR_CPU;i++){
		lists[i] = (void*)__get_free_pages(GFP_KERNEL,0);
		*((void**)lists[i]) = NULL;
	}
#endif

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

	unsigned long * p; // = (unsigned long*) sys_call_table;
	unsigned long cr0;
	int i;

#ifdef LOCAL
	for(i=0;i<NR_CPU;i++){
		free_pages((unsigned long)lists[i],0);
	}
#endif

	p = get_syscall_table();
        	
	printk("%s: shutting down\n",MODNAME);
	cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);
	for(i=0;i<HACKED_ENTRIES;i++){
		if (restore[i] != -1){
			p[restore[i]] = sys_ni_syscall_a;
		}
	}
	write_cr0(cr0);
	printk("%s: sys-call table restored to its original content\n",MODNAME);
	
}

