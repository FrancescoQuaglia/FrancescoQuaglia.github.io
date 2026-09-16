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
	{ 0xc0b13209, "try_module_get" },
	{ 0x2d4f4516, "param_ops_ulong" },
	{ 0xd272d446, "__SCT__preempt_schedule" },
	{ 0xa53f4e29, "memcpy" },
	{ 0xcb8b6ec6, "kfree" },
	{ 0x5d05e3c5, "pcpu_hot" },
	{ 0xd272d446, "__fentry__" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0xc9b02247, "const_pcpu_hot" },
	{ 0x01fa7dc1, "module_put" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0xcdec1689, "tasklet_init" },
	{ 0xbd03ed67, "random_kmalloc_seed" },
	{ 0x9c0551c6, "__tasklet_schedule" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xa7a0de59, "pv_ops" },
	{ 0xe18fddbc, "__kmalloc_cache_noprof" },
	{ 0xd272d446, "BUG_func" },
	{ 0xfc961df9, "kmalloc_caches" },
	{ 0xd268ca91, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xc0b13209,
	0x2d4f4516,
	0xd272d446,
	0xa53f4e29,
	0xcb8b6ec6,
	0x5d05e3c5,
	0xd272d446,
	0xe8213e80,
	0xd272d446,
	0xc9b02247,
	0x01fa7dc1,
	0x90a48d82,
	0xcdec1689,
	0xbd03ed67,
	0x9c0551c6,
	0xd272d446,
	0xa7a0de59,
	0xe18fddbc,
	0xd272d446,
	0xfc961df9,
	0xd268ca91,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"try_module_get\0"
	"param_ops_ulong\0"
	"__SCT__preempt_schedule\0"
	"memcpy\0"
	"kfree\0"
	"pcpu_hot\0"
	"__fentry__\0"
	"_printk\0"
	"__stack_chk_fail\0"
	"const_pcpu_hot\0"
	"module_put\0"
	"__ubsan_handle_out_of_bounds\0"
	"tasklet_init\0"
	"random_kmalloc_seed\0"
	"__tasklet_schedule\0"
	"__x86_return_thunk\0"
	"pv_ops\0"
	"__kmalloc_cache_noprof\0"
	"BUG_func\0"
	"kmalloc_caches\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "50675D40641CA7D26510EC4");
