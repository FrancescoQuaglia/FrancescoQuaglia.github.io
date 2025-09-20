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
	"\x1c\x00\x00\x00\xc4\x45\x5f\xbc"
	"inode_init_owner\0\0\0\0"
	"\x14\x00\x00\x00\xaa\xa1\x86\x46"
	"iget_locked\0"
	"\x1c\x00\x00\x00\x48\x9f\xdb\x88"
	"__check_object_size\0"
	"\x18\x00\x00\x00\xc2\x9c\xc4\x13"
	"_copy_from_user\0"
	"\x20\x00\x00\x00\x4a\x8d\x71\x34"
	"unregister_filesystem\0\0\0"
	"\x14\x00\x00\x00\x91\xe9\x08\x69"
	"d_make_root\0"
	"\x1c\x00\x00\x00\x02\xf2\x87\x8e"
	"write_dirty_buffer\0\0"
	"\x10\x00\x00\x00\xa0\x49\x81\x56"
	"iput\0\0\0\0"
	"\x1c\x00\x00\x00\x7c\x16\x02\xb3"
	"register_filesystem\0"
	"\x14\x00\x00\x00\xbb\x6d\xfb\xbd"
	"__fentry__\0\0"
	"\x24\x00\x00\x00\x97\x70\x48\x65"
	"__x86_indirect_thunk_rax\0\0\0\0"
	"\x1c\x00\x00\x00\x77\x23\x9a\x24"
	"kill_block_super\0\0\0\0"
	"\x1c\x00\x00\x00\x35\x69\xbe\x5b"
	"unlock_new_inode\0\0\0\0"
	"\x10\x00\x00\x00\x7e\x3a\x2c\x12"
	"_printk\0"
	"\x1c\x00\x00\x00\xcb\xf6\xfd\xf0"
	"__stack_chk_fail\0\0\0\0"
	"\x14\x00\x00\x00\x7f\x49\xaf\x24"
	"__brelse\0\0\0\0"
	"\x14\x00\x00\x00\xd2\x19\xbc\x57"
	"down_write\0\0"
	"\x14\x00\x00\x00\x25\x7a\x80\xce"
	"up_write\0\0\0\0"
	"\x14\x00\x00\x00\x0d\xbc\xd0\x7c"
	"set_nlink\0\0\0"
	"\x1c\x00\x00\x00\x96\xca\xc6\x9e"
	"ktime_get_real_ts64\0"
	"\x14\x00\x00\x00\x87\x9b\x08\xcc"
	"__bread_gfp\0"
	"\x1c\x00\x00\x00\xca\x39\x82\x5b"
	"__x86_return_thunk\0\0"
	"\x18\x00\x00\x00\xe1\xbe\x10\x6b"
	"_copy_to_user\0\0\0"
	"\x10\x00\x00\x00\x5a\x25\xd5\xe2"
	"strcmp\0\0"
	"\x10\x00\x00\x00\xa8\xcc\x9b\x07"
	"d_add\0\0\0"
	"\x14\x00\x00\x00\x7d\xf6\x4a\xa9"
	"mount_bdev\0\0"
	"\x14\x00\x00\x00\xf6\x5e\xb8\xd9"
	"lockref_get\0"
	"\x18\x00\x00\x00\x56\x41\x31\xbc"
	"nop_mnt_idmap\0\0\0"
	"\x18\x00\x00\x00\xd7\xd3\x75\x6d"
	"module_layout\0\0\0"
	"\x00\x00\x00\x00\x00\x00\x00\x00";

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "F7F493BC47688471A2C732F");
