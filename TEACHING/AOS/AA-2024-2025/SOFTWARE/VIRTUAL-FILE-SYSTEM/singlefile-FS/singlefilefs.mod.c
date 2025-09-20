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
	"\x1c\x00\x00\x00\x16\x34\x9d\x33"
	"inode_init_owner\0\0\0\0"
	"\x14\x00\x00\x00\xfa\xe9\xe2\xf2"
	"iget_locked\0"
	"\x1c\x00\x00\x00\x48\x9f\xdb\x88"
	"__check_object_size\0"
	"\x20\x00\x00\x00\x49\x0e\x55\xfa"
	"unregister_filesystem\0\0\0"
	"\x14\x00\x00\x00\x61\x1b\xa4\xe6"
	"d_make_root\0"
	"\x10\x00\x00\x00\x46\x95\x2a\x22"
	"iput\0\0\0\0"
	"\x1c\x00\x00\x00\x8b\xfe\x0d\xb4"
	"register_filesystem\0"
	"\x14\x00\x00\x00\xbb\x6d\xfb\xbd"
	"__fentry__\0\0"
	"\x24\x00\x00\x00\x97\x70\x48\x65"
	"__x86_indirect_thunk_rax\0\0\0\0"
	"\x1c\x00\x00\x00\x32\x47\xbd\xc8"
	"kill_block_super\0\0\0\0"
	"\x1c\x00\x00\x00\x1c\xd9\x50\x49"
	"unlock_new_inode\0\0\0\0"
	"\x10\x00\x00\x00\x7e\x3a\x2c\x12"
	"_printk\0"
	"\x1c\x00\x00\x00\xcb\xf6\xfd\xf0"
	"__stack_chk_fail\0\0\0\0"
	"\x14\x00\x00\x00\x98\x05\x65\xb6"
	"__brelse\0\0\0\0"
	"\x14\x00\x00\x00\xeb\x9c\x7e\x91"
	"set_nlink\0\0\0"
	"\x1c\x00\x00\x00\x96\xca\xc6\x9e"
	"ktime_get_real_ts64\0"
	"\x14\x00\x00\x00\x53\x43\x50\x54"
	"__bread_gfp\0"
	"\x1c\x00\x00\x00\xca\x39\x82\x5b"
	"__x86_return_thunk\0\0"
	"\x18\x00\x00\x00\xe1\xbe\x10\x6b"
	"_copy_to_user\0\0\0"
	"\x10\x00\x00\x00\x5a\x25\xd5\xe2"
	"strcmp\0\0"
	"\x10\x00\x00\x00\x10\x5a\xa2\x53"
	"d_add\0\0\0"
	"\x14\x00\x00\x00\x2f\x3f\x52\xee"
	"mount_bdev\0\0"
	"\x14\x00\x00\x00\xf6\x5e\xb8\xd9"
	"lockref_get\0"
	"\x18\x00\x00\x00\x56\x41\x31\xbc"
	"nop_mnt_idmap\0\0\0"
	"\x18\x00\x00\x00\x3a\x0a\xd8\xfc"
	"module_layout\0\0\0"
	"\x00\x00\x00\x00\x00\x00\x00\x00";

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "799CCDE15DF02A1A987A225");
