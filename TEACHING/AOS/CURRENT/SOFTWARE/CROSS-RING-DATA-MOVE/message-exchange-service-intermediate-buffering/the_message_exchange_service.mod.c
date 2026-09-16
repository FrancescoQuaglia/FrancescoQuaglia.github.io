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
	{ 0x2d4f4516, "param_ops_ulong" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0xd272d446, "__SCT__preempt_schedule" },
	{ 0xa53f4e29, "memcpy" },
	{ 0x5d05e3c5, "pcpu_hot" },
	{ 0x02e1dca7, "free_pages" },
	{ 0xd272d446, "__fentry__" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0x48fd0ca0, "get_zeroed_page_noprof" },
	{ 0xf46d5bf3, "mutex_lock" },
	{ 0xe54e0a6b, "__fortify_panic" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x092a35a2, "_copy_to_user" },
	{ 0xa7a0de59, "pv_ops" },
	{ 0xf46d5bf3, "mutex_unlock" },
	{ 0xd272d446, "BUG_func" },
	{ 0xd268ca91, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xa61fd7aa,
	0x2d4f4516,
	0x092a35a2,
	0xd272d446,
	0xa53f4e29,
	0x5d05e3c5,
	0x02e1dca7,
	0xd272d446,
	0xe8213e80,
	0xd272d446,
	0x90a48d82,
	0x48fd0ca0,
	0xf46d5bf3,
	0xe54e0a6b,
	0xd272d446,
	0x092a35a2,
	0xa7a0de59,
	0xf46d5bf3,
	0xd272d446,
	0xd268ca91,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__check_object_size\0"
	"param_ops_ulong\0"
	"_copy_from_user\0"
	"__SCT__preempt_schedule\0"
	"memcpy\0"
	"pcpu_hot\0"
	"free_pages\0"
	"__fentry__\0"
	"_printk\0"
	"__stack_chk_fail\0"
	"__ubsan_handle_out_of_bounds\0"
	"get_zeroed_page_noprof\0"
	"mutex_lock\0"
	"__fortify_panic\0"
	"__x86_return_thunk\0"
	"_copy_to_user\0"
	"pv_ops\0"
	"mutex_unlock\0"
	"BUG_func\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "3C1566572ACC10041275F3D");
