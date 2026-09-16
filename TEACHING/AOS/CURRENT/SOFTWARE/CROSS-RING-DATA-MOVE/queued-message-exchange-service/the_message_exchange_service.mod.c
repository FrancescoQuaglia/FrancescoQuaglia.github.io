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
	{ 0xde338d9a, "_raw_spin_lock" },
	{ 0xd272d446, "__fentry__" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0xf64ac983, "__copy_overflow" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0x83db07b4, "kmem_cache_free" },
	{ 0xe54e0a6b, "__fortify_panic" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xb75420f8, "kmem_cache_alloc_noprof" },
	{ 0x092a35a2, "_copy_to_user" },
	{ 0x12786fd0, "__kmem_cache_create_args" },
	{ 0xa7a0de59, "pv_ops" },
	{ 0xde338d9a, "_raw_spin_unlock" },
	{ 0xd272d446, "BUG_func" },
	{ 0xc5dfa1cd, "kmem_cache_destroy" },
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
	0xde338d9a,
	0xd272d446,
	0xe8213e80,
	0xd272d446,
	0xf64ac983,
	0x90a48d82,
	0x83db07b4,
	0xe54e0a6b,
	0xd272d446,
	0xb75420f8,
	0x092a35a2,
	0x12786fd0,
	0xa7a0de59,
	0xde338d9a,
	0xd272d446,
	0xc5dfa1cd,
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
	"_raw_spin_lock\0"
	"__fentry__\0"
	"_printk\0"
	"__stack_chk_fail\0"
	"__copy_overflow\0"
	"__ubsan_handle_out_of_bounds\0"
	"kmem_cache_free\0"
	"__fortify_panic\0"
	"__x86_return_thunk\0"
	"kmem_cache_alloc_noprof\0"
	"_copy_to_user\0"
	"__kmem_cache_create_args\0"
	"pv_ops\0"
	"_raw_spin_unlock\0"
	"BUG_func\0"
	"kmem_cache_destroy\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "967CEC4FB4E47E3FD5F88A8");
