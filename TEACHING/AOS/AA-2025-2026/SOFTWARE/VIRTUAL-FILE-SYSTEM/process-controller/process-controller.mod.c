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
	{ 0xa61fd7aa, "__check_object_size" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0x1f55c5b2, "kstrtoll" },
	{ 0x4d40f3a6, "const_pcpu_hot" },
	{ 0xd272d446, "__rcu_read_lock" },
	{ 0xcde75db5, "find_vpid" },
	{ 0x774b89a0, "pid_task" },
	{ 0xe8d8d116, "get_task_mm" },
	{ 0xd272d446, "__rcu_read_unlock" },
	{ 0xbd03ed67, "page_offset_base" },
	{ 0x092a35a2, "_copy_to_user" },
	{ 0x397daafe, "mmput" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0xbd03ed67, "phys_base" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0x11169e9e, "__register_chrdev" },
	{ 0x52b15b3b, "__unregister_chrdev" },
	{ 0xd272d446, "__fentry__" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xe8213e80, "_printk" },
	{ 0xba157484, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xa61fd7aa,
	0x092a35a2,
	0x1f55c5b2,
	0x4d40f3a6,
	0xd272d446,
	0xcde75db5,
	0x774b89a0,
	0xe8d8d116,
	0xd272d446,
	0xbd03ed67,
	0x092a35a2,
	0x397daafe,
	0x90a48d82,
	0xbd03ed67,
	0xd272d446,
	0x11169e9e,
	0x52b15b3b,
	0xd272d446,
	0xd272d446,
	0xe8213e80,
	0xba157484,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__check_object_size\0"
	"_copy_from_user\0"
	"kstrtoll\0"
	"const_pcpu_hot\0"
	"__rcu_read_lock\0"
	"find_vpid\0"
	"pid_task\0"
	"get_task_mm\0"
	"__rcu_read_unlock\0"
	"page_offset_base\0"
	"_copy_to_user\0"
	"mmput\0"
	"__ubsan_handle_out_of_bounds\0"
	"phys_base\0"
	"__stack_chk_fail\0"
	"__register_chrdev\0"
	"__unregister_chrdev\0"
	"__fentry__\0"
	"__x86_return_thunk\0"
	"_printk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "2F7180381EC335BB753E152");
