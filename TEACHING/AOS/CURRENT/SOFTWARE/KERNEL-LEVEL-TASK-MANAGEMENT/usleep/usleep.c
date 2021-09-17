#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
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


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");
MODULE_DESCRIPTION("this module implements a sleep/wakeup FIFO queue with amortized O(1) kernel level complexity");

//NOTE: module locking is not explicit - you should implement explicit locking by your own if needed 

#define MODNAME "SLEEP/WAKEUP QUEUE"

//#define CLASSIC

#define NO (0)
#define YES (NO+1)
#define AUDIT if(0)

int restore[2] = {[0 ... 1] -1};//can hack up to 2 system call table entries - one for the sleep service one for the awake service


unsigned long sys_call_table;// position of the syscall table - to be discovered at startup
unsigned long sys_ni_syscall_a;// position of the sys_ni_syscall code - to be discovred at startup



typedef struct _control_record{
	struct task_struct *task;	
	int pid;
	int awake;
	struct hrtimer hr_timer;
} control_record;


static enum hrtimer_restart my_hrtimer_callback( struct hrtimer *timer ){

	control_record *control;
	struct task_struct *the_task;

	control = (control_record*)container_of(timer,control_record, hr_timer);
	control->awake = YES;
	the_task = control->task;
	wake_up_process(the_task);

	return HRTIMER_NORESTART;
}




asmlinkage long sys_goto_sleep(unsigned long microsecs){
	

	struct hrtimer* hr_timer;
	control_record* control;
	ktime_t ktime_interval;

	if(microsecs == 0) return 0;

	AUDIT
	printk("%s: thread %d going to usleep for %lu microsecs\n",MODNAME,current->pid,microsecs);

	ktime_interval = ktime_set( 0, microsecs*1000 );

	DECLARE_WAIT_QUEUE_HEAD(the_queue);//here we use a private queue - wakeup is selective via wake_up_process


#ifdef CLASSIC
        control = (control_record*)((void*)current->stack + sizeof(struct thread_info));
        control->awake = NO;
	wait_event_interruptible_hrtimeout(the_queue, control->awake == YES, ktime_interval);
	return 0;
#endif

        control = (control_record*)((void*)current->stack + sizeof(struct thread_info));

	control->task = current;
	control->pid  = current->pid;
	control->awake = NO;

        //hr_timer = (struct hrtimer*)((void*)current->stack + sizeof(struct thread_info));
	hrtimer_init(&(control->hr_timer), CLOCK_MONOTONIC, HRTIMER_MODE_REL);

	control->hr_timer.function = &my_hrtimer_callback;
	hrtimer_start(&(control->hr_timer), ktime_interval, HRTIMER_MODE_REL);

		
	wait_event_interruptible(the_queue, control->awake == YES);

	hrtimer_cancel(&(control->hr_timer));
	
	AUDIT
	printk("%s: thread %d exiting usleep\n",MODNAME, current->pid);

	return 0;

}


unsigned long *get_syscall_table(void) {       

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

	unsigned long * p;
	int i,j;
	int ret;
	unsigned long target;

	unsigned long cr0;

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
			if (j == 0) break;
		}
	}

	cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);
	p[restore[0]] = (unsigned long)sys_goto_sleep;
	write_cr0(cr0);

	printk("%s: new system-call sys_goto_sleep installed on sys-call table entry %d\n",MODNAME,restore[0]);

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
	write_cr0(cr0);
	printk("%s: sys-call table restored to its original content\n",MODNAME);
	
}

