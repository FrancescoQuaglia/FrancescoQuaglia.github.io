#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/workqueue.h>


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
MODULE_DESCRIPTION("basic example usage of work queues");

#define MODNAME "WORK QUEUES"

#define AUDIT if(1)

int restore[2] = {[0 ... 1] -1}; //can hack up to 2 system call table entries

unsigned long sys_call_table;// position of the syscall table - to be discovered at startup
unsigned long sys_ni_syscall_a;// position of the sys_ni_syscall code - to be discovred at startup



typedef struct _packed_work{
	void* buffer;	
	long code;
	struct work_struct the_work;
} packed_work;

void audit(unsigned long data){
	
	AUDIT{
		printk("%s: ------------------------------\n",MODNAME);
		printk("%s: this print comes from kworker daemon with PID=%d - running on CPU-core %d\n",MODNAME,current->pid,smp_processor_id());
	}

	AUDIT
	printk("%s: running task with code  %ld\n",MODNAME,container_of(data,packed_work,the_work)->code);

	AUDIT
	printk("%s: releasing the task buffer at address %p - container of task is at %p\n",MODNAME,(void*)data,container_of(data,packed_work,the_work));


	kfree((void*)container_of(data,packed_work,the_work));

	module_put(THIS_MODULE);

}


//this function emulates a top half  
//NOTE: the structure is still not compliant with non-preemptability and standing interrupts 
//that would be requested by, e.g., a real top half in a device driver
asmlinkage long sys_put_work(int core, int request_code){

	packed_work *the_task;
	
	if(!try_module_get(THIS_MODULE)) return -1;

	AUDIT{
		printk("%s: ------------------------\n",MODNAME);
		printk("%s: requested deferred work with request code: %d\n",MODNAME,request_code);
	}

	the_task = kzalloc(sizeof(packed_work),GFP_ATOMIC);//non blocking memory allocation

	if (the_task == NULL) {
		printk("%s: tasklet buffer allocation failure\n",MODNAME);
		module_put(THIS_MODULE);
		return -1;
	}

	the_task->buffer = the_task;
	the_task->code = request_code;
	
	AUDIT
	printk("%s: work buffer allocation success - address is %p\n",MODNAME,the_task);


	__INIT_WORK(&(the_task->the_work),(void*)audit,&(the_task->the_work));
	
	schedule_work_on(core,&the_task->the_work);

	return 0;
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

	unsigned long * p;// = (unsigned long *) sys_call_table;
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
	p[restore[0]] = (unsigned long)sys_put_work;
	write_cr0(cr0);

	printk("%s: new system-call sys_put_work installed on sys-call table entry %d\n",MODNAME,restore[0]);

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

