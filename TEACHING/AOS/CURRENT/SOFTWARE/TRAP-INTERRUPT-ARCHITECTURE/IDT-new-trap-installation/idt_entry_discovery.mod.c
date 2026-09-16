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
	{ 0x4d40f3a6, "const_pcpu_hot" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x4ff89cd9, "pcpu_hot" },
	{ 0xb3f8c2e9, "pv_ops" },
	{ 0xd272d446, "BUG_func" },
	{ 0xd272d446, "__SCT__preempt_schedule" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0xba157484, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xd272d446,
	0x4d40f3a6,
	0xe8213e80,
	0xd272d446,
	0x4ff89cd9,
	0xb3f8c2e9,
	0xd272d446,
	0xd272d446,
	0xd272d446,
	0xba157484,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__fentry__\0"
	"const_pcpu_hot\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"pcpu_hot\0"
	"pv_ops\0"
	"BUG_func\0"
	"__SCT__preempt_schedule\0"
	"__stack_chk_fail\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "F99E09AB1ACF5268FB04696");
