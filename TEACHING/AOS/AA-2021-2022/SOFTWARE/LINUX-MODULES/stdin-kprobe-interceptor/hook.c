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
MODULE_DESCRIPTION("This module intecepts the return of the sys_read kernel function for a target process\
	it then audits a maximum of 128 read bytes into the dmesg buffer");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
#define target_func "__x64_sys_read"
#else
#define target_func "sys_read"
#endif 

unsigned long audit_counter = 0;//just to audit how manu times the interceptor function has been called
module_param(audit_counter, ulong, S_IRUSR | S_IRGRP | S_IROTH);

long target_pid = -1;//the target process id - please setup it via the /sys file system
module_param(target_pid, long, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);


static int the_hook(struct kprobe *ri, struct pt_regs *the_regs) { 

	char buffer[128];
	int  string_terminator = 128;
	int  i;
	int  ret;
	struct pt_regs *regs;


#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
	regs = the_regs->di;
#else
	regs = the_regs;
#endif


	atomic_inc((atomic_t*)&audit_counter);
	

	if (current->pid == (pid_t)target_pid){
		//get basic info on the executed sys_read
		printk("process %d read - channel is %ld - address is %p - requested num bytes is %ld\n",(int)target_pid,regs->di,(void*)regs->si,regs->dx);

		if(regs->di != 0) return 0;

		ret = copy_from_user((void*)buffer,(void*)regs->si,string_terminator);
		buffer[string_terminator -1 -ret] = '\0';
		
		//actually audit the delivered content - never more than one line
		printk(KERN_CONT "buffer content is: ");
		i = 0;
		while(buffer[i] != '\n' && buffer[i] != '\0')
			printk(KERN_CONT "%c",buffer[i++]);
		printk("\n");
	}


	return 0;
}

static struct kprobe kp = {
	.symbol_name = 	target_func,
	.post_handler = (kprobe_post_handler_t)the_hook,
};

static int  hook_init(void) {

	int ret;

	ret = register_kprobe(&kp);
	if (ret < 0) {
		pr_info("hook init failed, returned %d\n", ret);
		return ret;
	}
	printk("hook module correctly loaded\n");
	
	return 0;
}// hook_init

static void  hook_exit(void) {

	unregister_kprobe(&kp);

	printk("Hook invoked %lu times\n", audit_counter);

	printk("hook module unloaded\n");

}// hook_exit

module_init(hook_init)
module_exit(hook_exit)
MODULE_LICENSE("GPL");
