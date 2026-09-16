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
	{ 0x4d40f3a6, "const_pcpu_hot" },
	{ 0x7c898386, "register_kretprobe" },
	{ 0x8a7140c6, "smp_call_function" },
	{ 0x2182515b, "__num_online_cpus" },
	{ 0x1418bda3, "unregister_kretprobe" },
	{ 0xd272d446, "__SCT__preempt_schedule" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0xc2614bbe, "param_ops_long" },
	{ 0xc2614bbe, "param_ops_ulong" },
	{ 0xd272d446, "__fentry__" },
	{ 0x4ff89cd9, "pcpu_hot" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xba157484, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x4d40f3a6,
	0x7c898386,
	0x8a7140c6,
	0x2182515b,
	0x1418bda3,
	0xd272d446,
	0x092a35a2,
	0xd272d446,
	0xc2614bbe,
	0xc2614bbe,
	0xd272d446,
	0x4ff89cd9,
	0xe8213e80,
	0xd272d446,
	0xba157484,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"const_pcpu_hot\0"
	"register_kretprobe\0"
	"smp_call_function\0"
	"__num_online_cpus\0"
	"unregister_kretprobe\0"
	"__SCT__preempt_schedule\0"
	"_copy_from_user\0"
	"__stack_chk_fail\0"
	"param_ops_long\0"
	"param_ops_ulong\0"
	"__fentry__\0"
	"pcpu_hot\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "00B289C42E87029347D4572");
