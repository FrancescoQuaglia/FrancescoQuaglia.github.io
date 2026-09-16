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
	{ 0xe8213e80, "_printk" },
	{ 0x8a832107, "kernel_kobj" },
	{ 0xdec6d135, "kobject_create_and_add" },
	{ 0x8904df5e, "sysfs_create_file_ns" },
	{ 0xe125349f, "kobject_put" },
	{ 0xc2614bbe, "param_ops_int" },
	{ 0xd272d446, "__fentry__" },
	{ 0x173ec8da, "sscanf" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xdd6830c7, "sprintf" },
	{ 0xba157484, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xe8213e80,
	0x8a832107,
	0xdec6d135,
	0x8904df5e,
	0xe125349f,
	0xc2614bbe,
	0xd272d446,
	0x173ec8da,
	0xd272d446,
	0xdd6830c7,
	0xba157484,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"_printk\0"
	"kernel_kobj\0"
	"kobject_create_and_add\0"
	"sysfs_create_file_ns\0"
	"kobject_put\0"
	"param_ops_int\0"
	"__fentry__\0"
	"sscanf\0"
	"__x86_return_thunk\0"
	"sprintf\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "6319F1634CDF5625CBEB8AA");
