#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/ktime.h>
#include <linux/limits.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/printk.h>      
#include <linux/ptrace.h>       
#include <linux/syscalls.h>
#include <linux/version.h>


MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");
MODULE_DESCRIPTION("This module intecepts the return of the sys_read kernel function from standard-input\
	       		for a target process it then audits the read bytes into the dmesg buffer");

#define MODNAME "READ-INTERCEPTOR"

//what function to hook depending on the system call architecture
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
#define target_func "__x64_sys_read"
#else
#define target_func "sys_read"
#endif 

//the function to hook at startup for setting up per-cpu variables via kprobe
#define setup_target_func "run_on_cpu"

//where to look at when searching system call parmeters
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
#define get(regs)	regs = (struct pt_regs*)the_regs->di;
#else
#define get(regs)	regs = the_regs;
#endif

//offset for storing information on the top part of thread stack area
#ifdef CONFIG_THREAD_INFO_IN_TASK
int offset = sizeof(struct thread_info);
#else
int offset = 0;
#endif

//where to store the address of the user level buffer for the read operation depends on the CONFIG_THREAD_INFO_IN_TASK kernel compile time parameter
#define store_address(addr) *(unsigned long*)((void*)current->stack + offset) = addr
#define load_address(addr) addr = (*(unsigned long*)((void*)current->stack + offset))

static struct kretprobe setup_probe;  
static struct kretprobe retprobe;  
static struct kretprobe *the_retprobe = &setup_probe;  


unsigned long audit_counter = 0;//just to audit how manu times the interceptor hook (the return krobe) has been called
module_param(audit_counter, ulong, S_IRUSR | S_IRGRP | S_IROTH);

long target_pid = -1;//the target process id to intercept - please set it up via the /sys file system
module_param(target_pid, long, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);


void run_on_cpu(void* x){//this is here just to enable a kprobe on it 
	printk("%s: running on CPU %d\n", MODNAME, smp_processor_id());
	return;
}

unsigned long successful_search_counter = 0;//number of CPUs that succesfully found the address of the per-CPU variable that keeps the reference to the current kprobe context 

DEFINE_PER_CPU(unsigned long, BRUTE_START);//redundant you might use the below per-cpu variable to setup the initial searhc address
DEFINE_PER_CPU(unsigned long *, kprobe_context_pointer);//this is used for steady state operations 

unsigned long* reference_offset = 0x0;

static int the_search(struct kretprobe_instance *ri, struct pt_regs *the_regs) { 

	unsigned long* temp = (unsigned long)&BRUTE_START;

	printk("%s: running the brute force search on CPU %d\n", MODNAME, smp_processor_id());

	while (temp > 0) {//brute force search of the current_kprobe per-CPU variable
			  //for enabling blocking execution of the kretprobe
			  //you can save this time setting up a per CPU-variable via 
			  //smp_call_function() upon module startup
                temp -= 1; 
#ifndef CONFIG_KRETPROBE_ON_RETHOOK
                if ((unsigned long) __this_cpu_read(*temp) == (unsigned long) &ri->rp->kp) {
#else
                if ((unsigned long) __this_cpu_read(*temp) == (unsigned long) &the_retprobe->kp) {
#endif
			atomic_inc((atomic_t*)&successful_search_counter);//mention we have found the target 
			printk("%s: found the target per-cpu variable (CPU %d) - offset is %p\n", MODNAME, smp_processor_id(),temp);
			reference_offset = temp;//this assignment is done by multiple threads with no problem
                        break;
                }
		if(temp <= 0) return 1;
        }

	__this_cpu_write(kprobe_context_pointer, temp);

	return 0;
}


//this function is executed at the startup of the kretprobed one - we record the address of the atarget user space buffer in a per thread location on the top of the stack area
static int the_pre_hook(struct kprobe *ri, struct pt_regs *the_regs) { 

	struct pt_regs * regs;
	unsigned long addr; 

	atomic_inc((atomic_t*)&audit_counter);//just mention we passed here via the atomic counter

	store_address(0x0);	

	get(regs);//get the actual address of the CPU image seen by the system call (or its wrapper)

	if(regs->di != 0) return 1;//not reading from standard input
	if (current->pid != (pid_t)target_pid) return 1;//read not coming from the target process
							//here we only consider the original namespace of 
							//process identifiers

	addr = regs->si;
	printk("%s: pre-handler - target address is %px", MODNAME, (void*)addr);
	store_address(addr);

	return 0;//target process reading from standard input - the kretprobe needs to be executed
}



//in this kretprobe we reuse the address saved by the previous function to grub access to the bytes delivered to the user and log them via printk
static int the_hook(struct kretprobe_instance *ri, struct pt_regs *the_regs) { 

	unsigned long addr;
	char c;
	int  i;
	unsigned long* kprobe_cpu;

	//here we do not need to check if we are under interception of the read operation since this job has
	//already been done by the prehandler

        if (signal_pending(current)) {
                printk("%s: signal is pending for the current thread - tracing exit\n",MODNAME);
                goto end;
        }


	load_address(addr);
	if (addr == 0x0) goto end;//chek if we saved the address of the user level buffer for the I/O operation

	if (regs_return_value(the_regs) <= 0) goto end;//check if the read operation actually returned data

	printk("%s: thread %d got this from the read service on stdin (num bytes is %ld - target address is %px): ", MODNAME, current->pid, regs_return_value(the_regs), (void*)addr);

	kprobe_cpu = __this_cpu_read(kprobe_context_pointer);
	__this_cpu_write(*kprobe_cpu, NULL);
        preempt_enable();

	i = 0;
	while( !copy_from_user((void*)&c,(void*)(addr)+i,1) && (i < regs_return_value(the_regs))){
		       	printk(KERN_CONT "%c", c);
			++i;
	}
	printk("\n");

        preempt_disable();
	kprobe_cpu = __this_cpu_read(kprobe_context_pointer);

#ifndef CONFIG_KRETPROBE_ON_RETHOOK
        __this_cpu_write(*kprobe_cpu, &ri->rp->kp);
#else
        __this_cpu_write(*kprobe_cpu, &the_retprobe->kp);
#endif

end:

	return 0;
}


static int  hook_init(void) {

	int ret;

	if (LINUX_VERSION_CODE < KERNEL_VERSION(5,11,0)){
	 	printk("%s: unsupported kernel version", MODNAME);
		return -1; 	
	};

	setup_probe.kp.symbol_name = setup_target_func;
	setup_probe.handler = NULL;
	setup_probe.entry_handler = (kretprobe_handler_t)the_search;
	setup_probe.maxactive = -1; //lets' go for the default number of active kretprobes manageable by the kernel

	ret = register_kretprobe(&setup_probe);
	if (ret < 0) {
		printk("%s: hook init failed for the init kprobe setup, returned %d\n", MODNAME, ret);
		return ret;
	}

	get_cpu();//this is redundant but you could use the returned CPU-id
        smp_call_function(run_on_cpu,NULL,1);

	//run_on_cpu(NULL);
	

	if(successful_search_counter != (num_online_cpus() - 1)){
		printk("%s: read hook load failed - number of setup CPUs is %ld - number of remote online CPUs is %d\n", MODNAME, successful_search_counter, num_online_cpus() - 1);
		put_cpu();
	 	unregister_kretprobe(&setup_probe);
		return -1;

	}


	if (reference_offset == 0x0){
		printk("%s: inconsistemt value found for refeence offset\n", MODNAME);
		put_cpu();
	 	unregister_kretprobe(&setup_probe);
		return -1;
	}

	__this_cpu_write(kprobe_context_pointer, reference_offset);

	put_cpu();

	retprobe.kp.symbol_name = target_func;
	retprobe.handler = (kretprobe_handler_t)the_hook;
	retprobe.entry_handler = (kretprobe_handler_t)the_pre_hook;
	retprobe.maxactive = -1; //lets' go for the default number of active kretprobes manageable by the kernel

	ret = register_kretprobe(&retprobe);
	if (ret < 0) {
		printk("%s: hook init failed while setting up sys_read kprobe, returned %d\n", MODNAME, ret);
	 	unregister_kretprobe(&setup_probe);
		return ret;
	}

	printk("%s: read hook module correctly loaded\n", MODNAME);
	
	return 0;
}// hook_init

static void  hook_exit(void) {

	unregister_kretprobe(&setup_probe);
	unregister_kretprobe(&retprobe);

	printk("%s: read hook invoked %lu times\n",MODNAME,audit_counter);
	printk("%s: hook module unloaded\n",MODNAME);

}// hook_exit

module_init(hook_init)
module_exit(hook_exit)
MODULE_LICENSE("GPL");
