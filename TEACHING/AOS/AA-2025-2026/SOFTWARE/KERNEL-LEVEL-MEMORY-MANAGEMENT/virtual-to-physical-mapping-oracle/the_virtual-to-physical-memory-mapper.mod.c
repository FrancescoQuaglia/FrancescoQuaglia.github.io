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
	{ 0x1b3db703, "param_ops_ulong" },
	{ 0xd272d446, "__SCT__preempt_schedule" },
	{ 0xa53f4e29, "memcpy" },
	{ 0xd7a2edb5, "pcpu_hot" },
	{ 0xd272d446, "__fentry__" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0x8f2afe24, "const_pcpu_hot" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0xbd03ed67, "page_offset_base" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xa59da3c0, "down_read" },
	{ 0x821432fe, "pv_ops" },
	{ 0xa59da3c0, "up_read" },
	{ 0xd272d446, "BUG_func" },
	{ 0x70eca2ca, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x1b3db703,
	0xd272d446,
	0xa53f4e29,
	0xd7a2edb5,
	0xd272d446,
	0xe8213e80,
	0xd272d446,
	0x8f2afe24,
	0x90a48d82,
	0xbd03ed67,
	0xd272d446,
	0xa59da3c0,
	0x821432fe,
	0xa59da3c0,
	0xd272d446,
	0x70eca2ca,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"param_ops_ulong\0"
	"__SCT__preempt_schedule\0"
	"memcpy\0"
	"pcpu_hot\0"
	"__fentry__\0"
	"_printk\0"
	"__stack_chk_fail\0"
	"const_pcpu_hot\0"
	"__ubsan_handle_out_of_bounds\0"
	"page_offset_base\0"
	"__x86_return_thunk\0"
	"down_read\0"
	"pv_ops\0"
	"up_read\0"
	"BUG_func\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "B21E37BB3BF4F773CB90795");
