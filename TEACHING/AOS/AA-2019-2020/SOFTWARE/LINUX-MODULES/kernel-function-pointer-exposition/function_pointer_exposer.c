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
#include <linux/kthread.h>
#include <asm/page.h>
#include <asm/cacheflush.h>
#include <asm/apic.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <quaglia@dis.uniroma1.it>");
MODULE_DESCRIPTION("simply exposes a kernel level function pointer");

#define MODNAME "FUNCTION POINTER EXPOSER"

#define hook_dependency

unsigned long the_hook;
module_param(the_hook,ulong,0770);

#define THRESHOLD 1000

ulong passages __attribute__ ((aligned (8)));

void f(void){

	if(current->pid == 0){
		printk("thread id %d found on CPU %d - TCB is located at address %p\n",current->pid,smp_processor_id(),current);
	}

	atomic_inc((atomic_t*)&passages);

	if(passages>THRESHOLD){
		printk("%s: function actually called\n",MODNAME);
		passages=0;
	}
}



int init_module(void) {

	int ret;


#ifdef hook_dependency
	struct module *the_module;

	the_module = find_module("hook");	

	if (the_module == NULL) return -1;

	if(!try_module_get(the_module)) return -1;
#endif

	passages = 0;

	the_hook = (unsigned long)f;

	ret = 0;

	printk("%s: module mounted\n",MODNAME);

	return ret;
}


void cleanup_module(void) {

	struct module *the_module;

#ifdef hook_dependency
	the_module = find_module("hook");	

	if (the_module != NULL) module_put(the_module);
#endif
	printk("%s: module unmounted\n",MODNAME);
}

