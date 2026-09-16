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
	{ 0xc2614bbe, "param_ops_ulong" },
	{ 0xd272d446, "__SCT__preempt_schedule" },
	{ 0xc87f4bab, "finish_wait" },
	{ 0x535f4f5f, "hrtimer_init" },
	{ 0xa53f4e29, "memcpy" },
	{ 0x4ff89cd9, "pcpu_hot" },
	{ 0x0db8d68d, "prepare_to_wait_event" },
	{ 0xd272d446, "__fentry__" },
	{ 0xda80ecf4, "wake_up_process" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "schedule" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0x4d40f3a6, "const_pcpu_hot" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0x7a5ffe84, "init_wait_entry" },
	{ 0x5fa07cc0, "hrtimer_start_range_ns" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xb3f8c2e9, "pv_ops" },
	{ 0x36a36ab1, "hrtimer_cancel" },
	{ 0xd272d446, "BUG_func" },
	{ 0x7851be11, "__SCT__might_resched" },
	{ 0xba157484, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xc2614bbe,
	0xd272d446,
	0xc87f4bab,
	0x535f4f5f,
	0xa53f4e29,
	0x4ff89cd9,
	0x0db8d68d,
	0xd272d446,
	0xda80ecf4,
	0xe8213e80,
	0xd272d446,
	0xd272d446,
	0x4d40f3a6,
	0x90a48d82,
	0x7a5ffe84,
	0x5fa07cc0,
	0xd272d446,
	0xb3f8c2e9,
	0x36a36ab1,
	0xd272d446,
	0x7851be11,
	0xba157484,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"param_ops_ulong\0"
	"__SCT__preempt_schedule\0"
	"finish_wait\0"
	"hrtimer_init\0"
	"memcpy\0"
	"pcpu_hot\0"
	"prepare_to_wait_event\0"
	"__fentry__\0"
	"wake_up_process\0"
	"_printk\0"
	"schedule\0"
	"__stack_chk_fail\0"
	"const_pcpu_hot\0"
	"__ubsan_handle_out_of_bounds\0"
	"init_wait_entry\0"
	"hrtimer_start_range_ns\0"
	"__x86_return_thunk\0"
	"pv_ops\0"
	"hrtimer_cancel\0"
	"BUG_func\0"
	"__SCT__might_resched\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "D1CEC638399655A89702094");
