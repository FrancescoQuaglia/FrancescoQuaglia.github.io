#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/ktime.h>
#include <linux/limits.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/printk.h>      
#include <linux/ptrace.h>       
#include <linux/smp.h>       
#include <linux/version.h>


#define MODNAME "CONTEXT SWITCH HOOK"

//please define here whatever kernel function you would like to hook, except those that are blacklisted
#if LINUX_VERSION_CODE < KERNEL_VERSION(6,0,0)
#define target_func "finish_task_switch"//you should also include finish_task_switch.cold for completeness of whathever functionality to be executed upon a context switch
#else
#define target_func "__schedule"
#endif


unsigned long hook_func = 0;
module_param(hook_func, ulong, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

unsigned long audit_counter = 0;
module_param(audit_counter, ulong, S_IRUSR | S_IRGRP | S_IROTH);


MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");
MODULE_DESCRIPTION("This module will execute a kretprobe\
	just upon the completion of some target kernel function \
	identified by the target_function variable");


/*
 * this hook calls the function whose address
 * is contained in the hook_func variable
 * which is configurable as a moule param
 */

#define THRESHOLD 1000

int messaging_counter = 0;

static int tail_hook(struct kretprobe_instance *ri, struct pt_regs *regs) {//this is the kretprobe

	atomic_inc((atomic_t*)&audit_counter);
	
	messaging_counter++;
	if (messaging_counter > THRESHOLD) {
		messaging_counter = 0;
		printk("%s: at least %ld context-switch events have occurred - thread %d has been currently resheduled on CPU", MODNAME, audit_counter,current->pid);
	}

	return 0;
}

static struct kretprobe krp = {
	.handler                = tail_hook,
};

static int __init hook_init(void) {

	int ret;

	krp.kp.symbol_name = target_func;
	ret = register_kretprobe(&krp);
	if (ret < 0) {
		pr_info("%s: hook init failed, returned %d\n", MODNAME, ret);
		return ret;
	}
	printk("%s: hook module correctly loaded\n",MODNAME);
	
	return 0;
}// hook_init

static void __exit hook_exit(void) {

	unregister_kretprobe(&krp);

	printk("%s: hook invoked %lu times", MODNAME, audit_counter);
	printk("%s: hook module unloaded", MODNAME);

}// hook_exit

module_init(hook_init)
module_exit(hook_exit)
MODULE_LICENSE("GPL");
