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
	{ 0xf46d5bf3, "mutex_lock" },
	{ 0xa61fd7aa, "__check_object_size" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0xf46d5bf3, "mutex_unlock" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xf64ac983, "__copy_overflow" },
	{ 0xe8213e80, "_printk" },
	{ 0x33c78c8a, "remove_proc_entry" },
	{ 0x092a35a2, "_copy_to_user" },
	{ 0x3df0a3ce, "proc_create_data" },
	{ 0xba157484, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xd272d446,
	0xf46d5bf3,
	0xa61fd7aa,
	0x092a35a2,
	0xf46d5bf3,
	0xd272d446,
	0xf64ac983,
	0xe8213e80,
	0x33c78c8a,
	0x092a35a2,
	0x3df0a3ce,
	0xba157484,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__fentry__\0"
	"mutex_lock\0"
	"__check_object_size\0"
	"_copy_from_user\0"
	"mutex_unlock\0"
	"__x86_return_thunk\0"
	"__copy_overflow\0"
	"_printk\0"
	"remove_proc_entry\0"
	"_copy_to_user\0"
	"proc_create_data\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "21D6CE24F3CDFD65B6BB76A");
