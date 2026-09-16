#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xd272d446, "__fentry__" },
	{ 0xb2fa43dd, "kernel_sigaction" },
	{ 0x4d40f3a6, "const_pcpu_hot" },
	{ 0xc87f4bab, "finish_wait" },
	{ 0x4ff89cd9, "pcpu_hot" },
	{ 0xe8213e80, "_printk" },
	{ 0x7851be11, "__SCT__might_resched" },
	{ 0x7a5ffe84, "init_wait_entry" },
	{ 0x0db8d68d, "prepare_to_wait_event" },
	{ 0x6ac784f4, "schedule_timeout" },
	{ 0xf987cad7, "module_put" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0xe175d7d6, "try_module_get" },
	{ 0x8fdc4711, "kthread_create_on_node" },
	{ 0xda80ecf4, "wake_up_process" },
	{ 0xc2614bbe, "param_ops_int" },
	{ 0xba157484, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xd272d446,
	0xb2fa43dd,
	0x4d40f3a6,
	0xc87f4bab,
	0x4ff89cd9,
	0xe8213e80,
	0x7851be11,
	0x7a5ffe84,
	0x0db8d68d,
	0x6ac784f4,
	0xf987cad7,
	0xd272d446,
	0xd272d446,
	0xe175d7d6,
	0x8fdc4711,
	0xda80ecf4,
	0xc2614bbe,
	0xba157484,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__fentry__\0"
	"kernel_sigaction\0"
	"const_pcpu_hot\0"
	"finish_wait\0"
	"pcpu_hot\0"
	"_printk\0"
	"__SCT__might_resched\0"
	"init_wait_entry\0"
	"prepare_to_wait_event\0"
	"schedule_timeout\0"
	"module_put\0"
	"__x86_return_thunk\0"
	"__stack_chk_fail\0"
	"try_module_get\0"
	"kthread_create_on_node\0"
	"wake_up_process\0"
	"param_ops_int\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "6B38116DA6B54A84DABEC03");
