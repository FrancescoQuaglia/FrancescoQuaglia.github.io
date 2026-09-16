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
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x1418bda3, "unregister_kretprobe" },
	{ 0xc9b02247, "const_pcpu_hot" },
	{ 0x2d4f4516, "param_ops_ulong" },
	{ 0xd272d446, "__fentry__" },
	{ 0x7c898386, "register_kretprobe" },
	{ 0xe8213e80, "_printk" },
	{ 0xd268ca91, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xd272d446,
	0x1418bda3,
	0xc9b02247,
	0x2d4f4516,
	0xd272d446,
	0x7c898386,
	0xe8213e80,
	0xd268ca91,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__x86_return_thunk\0"
	"unregister_kretprobe\0"
	"const_pcpu_hot\0"
	"param_ops_ulong\0"
	"__fentry__\0"
	"register_kretprobe\0"
	"_printk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "BF3FA67A8B80B7792EFF402");
