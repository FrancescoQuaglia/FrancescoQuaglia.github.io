#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/wait.h>
#include <linux/spinlock.h>

// This gives access to read_cr0() and write_cr0()
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,3,0)
    #include <asm/switch_to.h>
#else
    #include <asm/system.h>
#endif
#ifndef X86_CR0_WP
#define X86_CR0_WP 0x00010000
#endif


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");
MODULE_DESCRIPTION("this module implements a kernel level skeleton protocol for software level kernel coordination");

//NOTE: module locking is not explicit - you should implement explicit locking by your own if needed 

#define MODNAME "MASTER/SLAVE SYNCH"

#define NR_CORES 4 //do not be wrong here


atomic_t count __attribute__((aligned(64)));//this is used to audit how many CPU-cores are still involve in the synchronization phase 

atomic_t barrier __attribute__((aligned(64)));//this is used to stop all cores except the one applying the patch


DEFINE_PER_CPU(int, me);


static void patch_synch(void * arg){
	int *x;

	preempt_disable();
	printk("I'm thread %d\n",current->pid);
	preempt_enable();
	x = this_cpu_ptr(&me);
        *x = smp_processor_id();
	atomic_dec(&count);
	while (atomic_read(&barrier));
}

static void apply_patch(void){

	unsigned long cr0;

	cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);
	//do wat you plan in memory
	write_cr0(cr0);
}

int init_module(void) {

	printk("%s: initializing\n",MODNAME);

	atomic_set(&barrier,1);

	atomic_set(&count,NR_CORES);
	atomic_dec(&count);

	smp_call_function(patch_synch,NULL,0);

	apply_patch();

	atomic_set(&barrier,0);

	return 0;


}


void cleanup_module(void) {

	printk("%s: shutting down\n",MODNAME);

	//here you should be able to restore what yuo patched	

}

