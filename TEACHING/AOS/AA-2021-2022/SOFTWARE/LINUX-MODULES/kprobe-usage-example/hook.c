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


//please define here whatever kernel function you would like to hook, except those that are blacklisted
#define target_func "finish_task_switch"

#define AUDIT if(1)//audit the RAX value if required

typedef void h_func(void);

unsigned long hook_func = 0;
module_param(hook_func, ulong, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

unsigned long audit_counter = 0;
module_param(audit_counter, ulong, S_IRUSR | S_IRGRP | S_IROTH);


MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");
MODULE_DESCRIPTION("This module will execute a given function pointed by hook_funct\
	just upon the completion of some target kernel function \
	identified by the target_function variable");


/*
 * this hook calls the function whose address
 * is contained in the hook_func variable
 * which is configurable as a moule param
 */


static int tail_hook(struct kretprobe_instance *ri, struct pt_regs *regs) {

	h_func *hook;
	atomic_inc((atomic_t*)&audit_counter);

	AUDIT
	audit_counter++;

	if (!hook_func) goto end;


	hook = (h_func*) hook_func;
	// Execute the function
	hook();

end:
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
		pr_info("hook init failed, returned %d\n", ret);
		return ret;
	}
	printk("hook module correctly loaded\n");
	
	return 0;
}// hook_init

static void __exit hook_exit(void) {

	unregister_kretprobe(&krp);
	//Be carefull, this unregister assumes that none will need to run the hook function after this nodule
	//is unmounted

	printk("Hook invoked %lu times\n", audit_counter);

	printk("hook module unloaded\n");

}// hook_exit

module_init(hook_init)
module_exit(hook_exit)
MODULE_LICENSE("GPL");
