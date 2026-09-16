#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/ktime.h>
#include <linux/limits.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/printk.h>      
#include <linux/ptrace.h>       
#include <linux/version.h>


#include "../include/monitor.h"

//please define here whatever kernel function you would like to hook, except those that are blacklisted
//please define here whatever kernel function you would like to hook, except those that are blacklisted
#if LINUX_VERSION_CODE < KERNEL_VERSION(6,0,0)
#define target_func "finish_task_switch"
#define target_func_cold "finish_task_switch.cold"
#else
#define target_func "pick_next_task"
#endif

#define AUDIT if(0) //audit condition 


unsigned long audit_counter = 0;
module_param(audit_counter, ulong, S_IRUSR | S_IRGRP | S_IROTH);


MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");
MODULE_DESCRIPTION("This module flushes to the stack area (just below thread-info) the current CPU id and the number of CPU switches experienced by the thread");

//DEFINE_PER_CPU(int, me);

#ifdef CONFIG_THREAD_INFO_IN_TASK
int offset = sizeof(struct thread_info);
#else
int offset = 0; 
#endif


static int tail_hook(struct kretprobe_instance *ri, struct pt_regs *regs)
{

	monitor *p;
	int cpu_id;

	atomic_inc((atomic_t*)&audit_counter);

	p = (void*)current->stack + offset;

	cpu_id = smp_processor_id();//no longer needed if we turn  to per-CPU memory

	if(p->cpu_id != cpu_id){
		p->switches++;
	}

        p->cpu_id = cpu_id;

	AUDIT
	printk("cpuid value is %d\n",p->cpu_id);

	return 0;
}

static struct kretprobe krp = {
	.handler                = tail_hook,
};

#if LINUX_VERSION_CODE < KERNEL_VERSION(6,0,0)
static struct kretprobe krp_cold = {
	.handler                = tail_hook,
};
#endif

static int __init hook_init(void)
{
	int ret;

	krp.kp.symbol_name = target_func;
	ret = register_kretprobe(&krp);
	if (ret < 0) {
		pr_info("hook init failed, returned %d\n", ret);
		return ret;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(6,0,0)
	krp_cold.kp.symbol_name = target_func_cold;
	ret = register_kretprobe(&krp_cold);
	if (ret < 0) {
		unregister_kretprobe(&krp);
		pr_info("hook init failed for cold task switch, returned %d - running anyhow\n", ret);
	}
#endif

	printk("hook module correctly loaded\n");
	
	return 0;
}// hook_init

static void __exit hook_exit(void)
{
	unregister_kretprobe(&krp);
#if LINUX_VERSION_CODE < KERNEL_VERSION(6,0,0)
	unregister_kretprobe(&krp_cold);
#endif

	printk("Hook invoked %lu times\n", audit_counter);

	printk("hook module unloaded\n");

}// hook_exit

module_init(hook_init)
module_exit(hook_exit)
MODULE_LICENSE("GPL");
