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


#define NO (0)
#define YES (NO+1)
#define AUDIT if(1)

int restore[2] = {[0 ... 1] -1};//can hack up to 2 system call table entries - one for the sleep service one for the awake service


unsigned long sys_call_table;// position of the syscall table - to be discovered at startup
unsigned long sys_ni_syscall_a;// position of the sys_ni_syscall code - to be discovred at startup


static int enable_sleep = 1;// this can be configured at run time via the sys file system - 1 meas any sleeping thread is freezed
module_param(enable_sleep,int,0660);

unsigned long count __attribute__((aligned(8)));//this is used to audit how many threads are still sleeping onto the sleep/wakeup queue
module_param(count,ulong,0660);

typedef struct _elem{
	struct task_struct *task;	
	int pid;
	int awake;
	int already_hit;
	struct _elem * next;
	struct _elem * prev;
} elem;

elem head = {NULL,-1,-1,-1,NULL,NULL};
elem tail = {NULL,-1,-1,-1,NULL,NULL};
spinlock_t queue_lock; 



asmlinkage long sys_goto_sleep(void){
	

	volatile elem me;
	elem *aux;
	DECLARE_WAIT_QUEUE_HEAD(the_queue);//here we use a private queue - wakeup is selective via wake_up_process

	me.next = NULL;
	me.prev = NULL;
	me.task = current;
	me.pid  = current->pid;
	me.awake = NO;
	me.already_hit = NO;

	AUDIT
	printk("%s: sys_goto_sleep on strong fifo sleep/wakeup queue called from thread %d\n",MODNAME,current->pid);


	if(!enable_sleep){
		printk("%s: sys_goto_sleep - sleeping not currently enabled\n",MODNAME);
		return -1;
	}

	preempt_disable();
	spin_lock(&queue_lock);

	aux = &tail;
	if(aux->prev == NULL){
		spin_unlock(&queue_lock);
		preempt_enable();
		printk("%s: malformed sleep-wakeup-queue - service damaged\n",MODNAME);
		return -1;
	}

	aux->prev->next = &me;
	me.prev = aux->prev;
	aux->prev = &me;
	me.next = aux;

sleep:

	spin_unlock(&queue_lock);
	preempt_enable();

	atomic_inc((atomic_t*)&count);//a new sleeper 

	AUDIT
	printk("%s: thread %d actually going to sleep\n",MODNAME,current->pid);
		
	wait_event_interruptible(the_queue, me.awake == YES);
	
	preempt_disable();
	spin_lock(&queue_lock);

	aux = &head;

	if(aux == NULL){
		spin_unlock(&queue_lock);
		preempt_enable();
		printk("%s: malformed sleep-wakeup-queue upon wakeup - service damaged\n",MODNAME);
		return -1;
	}

	me.prev->next = me.next;//we know where we are thanks to double linkage
	me.next->prev = me.prev;

	spin_unlock(&queue_lock);
	preempt_enable();

	AUDIT
	printk("%s: thread %d exiting sleep for a wakeup or signal\n",MODNAME, current->pid);

	atomic_dec((atomic_t*)&count);//finally awaken

	if(me.awake == NO){
		AUDIT
		printk("%s: thread %d exiting sleep for signal\n",MODNAME, current->pid);
		return -EINTR;
	}


	return 0;

}

asmlinkage long sys_awake(void){
	
	struct task_struct *the_task;
	int its_pid = -1;
	elem *aux;


	printk("%s: sys_awake called from thread %d\n",MODNAME,current->pid);
	
	aux = &head;

	preempt_disable();
	spin_lock(&queue_lock);

	if(aux == NULL){
		spin_unlock(&queue_lock);
		preempt_enable();
		printk("%s: malformed sleep-wakeup-queue\n",MODNAME);
		return -1;
	}

	while(aux->next != &tail){

		AUDIT
		printk("%s: in cycle\n",MODNAME);

		if(aux->next->already_hit == NO){
			the_task = aux->next->task;
			aux->next->awake = YES;
			aux->next->already_hit = YES;
			its_pid = aux->next->pid;
			wake_up_process(the_task);
			goto awaken;
		}

		aux = aux->next;

	}	
	
	spin_unlock(&queue_lock);
	preempt_enable();
	
	return 0;

awaken:
	spin_unlock(&queue_lock);
	preempt_enable();

	AUDIT
	printk("%s: called the awake of thread %d\n",MODNAME,its_pid);

	return its_pid;

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

	spin_lock_init(&queue_lock);

	head.next = &tail;// setup initial double linked list 
	tail.prev = &head;

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
	p[restore[0]] = (unsigned long)sys_goto_sleep;
	p[restore[1]] = (unsigned long)sys_awake;
	write_cr0(cr0);

	printk("%s: new system-call sys_goto_sleep installed on sys-call table entry %d\n",MODNAME,restore[0]);
	printk("%s: new system-call sys_awake_pid installed on sys-call table entry %d\n",MODNAME,restore[1]);

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

