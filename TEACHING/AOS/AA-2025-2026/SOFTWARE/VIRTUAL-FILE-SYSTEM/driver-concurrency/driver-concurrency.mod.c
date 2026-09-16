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
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0x357aaab3, "mutex_trylock" },
	{ 0xf46d5bf3, "mutex_lock" },
	{ 0xa61fd7aa, "__check_object_size" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0x092a35a2, "_copy_to_user" },
	{ 0xc1e6c71e, "__mutex_init" },
	{ 0x6bded543, "get_free_pages_noprof" },
	{ 0x11169e9e, "__register_chrdev" },
	{ 0x02e1dca7, "free_pages" },
	{ 0x52b15b3b, "__unregister_chrdev" },
	{ 0xd272d446, "__fentry__" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xf46d5bf3, "mutex_unlock" },
	{ 0xba157484, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x90a48d82,
	0x357aaab3,
	0xf46d5bf3,
	0xa61fd7aa,
	0x092a35a2,
	0x092a35a2,
	0xc1e6c71e,
	0x6bded543,
	0x11169e9e,
	0x02e1dca7,
	0x52b15b3b,
	0xd272d446,
	0xe8213e80,
	0xd272d446,
	0xf46d5bf3,
	0xba157484,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__ubsan_handle_out_of_bounds\0"
	"mutex_trylock\0"
	"mutex_lock\0"
	"__check_object_size\0"
	"_copy_from_user\0"
	"_copy_to_user\0"
	"__mutex_init\0"
	"get_free_pages_noprof\0"
	"__register_chrdev\0"
	"free_pages\0"
	"__unregister_chrdev\0"
	"__fentry__\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"mutex_unlock\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "0E38A6B39B84355456AEE8C");
