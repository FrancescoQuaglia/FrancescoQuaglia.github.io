#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
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

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const char ____versions[]
__used __section("__versions") =
	"\x1c\x00\x00\x00\xca\x39\x82\x5b"
	"__x86_return_thunk\0\0"
	"\x1c\x00\x00\x00\xa3\x6b\x50\xc9"
	"register_kretprobe\0\0"
	"\x10\x00\x00\x00\x7e\x3a\x2c\x12"
	"_printk\0"
	"\x20\x00\x00\x00\xbb\xcc\x6b\x23"
	"unregister_kretprobe\0\0\0\0"
	"\x18\x00\x00\x00\x5a\x5f\xb5\x35"
	"param_ops_ulong\0"
	"\x14\x00\x00\x00\xbb\x6d\xfb\xbd"
	"__fentry__\0\0"
	"\x14\x00\x00\x00\xe8\x2a\x36\x1e"
	"pcpu_hot\0\0\0\0"
	"\x18\x00\x00\x00\x3a\x0a\xd8\xfc"
	"module_layout\0\0\0"
	"\x00\x00\x00\x00\x00\x00\x00\x00";

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "7445542069CF818C06A9796");
