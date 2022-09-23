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
MODULE_DESCRIPTION("this module implements a kernel level skeleton protocol for software level coordination");

//NOTE: module locking is not explicit - you should implement explicit locking by your own if needed 

#define MODNAME "MASTER/SLAVE SYNCH"


atomic_t count __attribute__((aligned(64)));//this is used to audit how many CPU-cores are still involve in the synchronization phase 

atomic_t barrier __attribute__((aligned(64)));//this is used to stop all cores except the one applying the patch


DEFINE_PER_CPU(int, me);

unsigned long NR_CORES;

unsigned long cr0;

static inline void write_cr0_forced(unsigned long val){
    unsigned long __force_order;

    /* __asm__ __volatile__( */
    asm volatile(
        "mov %0, %%cr0"
        : "+r"(val), "+m"(__force_order));
}

static inline void protect_memory(void){
    write_cr0_forced(cr0);
}

static inline void unprotect_memory(void){
    write_cr0_forced(cr0 & ~X86_CR0_WP);
}


static void patch_synch(void * arg){
	int *x;

	x = this_cpu_ptr(&me);
        *x = smp_processor_id();
	atomic_dec(&count);
	while (atomic_read(&barrier));
}

static void apply_patch(void){

	cr0 = read_cr0();
	unprotect_memory();
	//do what you plan in memory
	protect_memory();
}

int init_module(void) {

	int *x;

	NR_CORES = num_online_cpus();
	printk("%s: initializing (running with %lu CPUs)\n",MODNAME,NR_CORES);

	x = this_cpu_ptr(&me);
        *x = smp_processor_id();

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

	//here you should be able to restore what yuo patched if needed	

}

