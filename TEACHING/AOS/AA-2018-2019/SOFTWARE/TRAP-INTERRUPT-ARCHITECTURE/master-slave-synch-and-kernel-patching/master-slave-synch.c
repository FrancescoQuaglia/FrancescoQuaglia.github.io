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
MODULE_AUTHOR("Francesco Quaglia <quaglia@dis.uniroma1.it>");
MODULE_DESCRIPTION("this module implements a kernel level skeleton protocol for software level kernel coordination and simulates the appliciton of a patch to the schedule() funtion ");

//NOTE: module locking is not explicit - you should implement explicit locking by your own if needed 

#define MODNAME "MASTER/SLAVE SYNCH"

/* please take this value from the system map */
unsigned long the_scheduler = 0xffffffff8161e9e0;

#define BYTES_TO_PATCH (128)
char buff[BYTES_TO_PATCH];//buffer where to save the schedule part you are patching in the simulation

#define NR_CORES 2 //do not be wrong here


atomic_t count;//this is used to audit how many CPU-cores are still involve in the synchronization phase 

atomic_t barrier;//this is used to stop all cores except the one applying the patch

static void patch_synch(void * arg){
	atomic_dec(&count);
	while (atomic_read(&barrier));
}

static void apply_patch(void){
	unsigned long * p = (unsigned long *) schedule;
	unsigned long cr0;

	memcpy(buff,the_scheduler,BYTES_TO_PATCH);	//save the schedule() function image

	//destroy and restore schedule() image
	cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);
	memset(the_scheduler,'f',BYTES_TO_PATCH);//here the schedule() function is temporarily destroyed	
	memcpy(the_scheduler,buff,BYTES_TO_PATCH);//here it is restored
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

	//here you should be able to restore schedule() if it was not a patch simulation	

}

