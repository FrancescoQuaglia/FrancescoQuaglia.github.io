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
	{ 0x8471422e, "inode_init_owner" },
	{ 0xa5d2285c, "iget_locked" },
	{ 0xa61fd7aa, "__check_object_size" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0xb03fd70b, "unregister_filesystem" },
	{ 0x692d8520, "d_make_root" },
	{ 0x75f272a9, "write_dirty_buffer" },
	{ 0xd36b6cbe, "iput" },
	{ 0xb03fd70b, "register_filesystem" },
	{ 0xd272d446, "__fentry__" },
	{ 0x5a844b26, "__x86_indirect_thunk_rax" },
	{ 0x9174f503, "kill_block_super" },
	{ 0xd36b6cbe, "unlock_new_inode" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0x1dd0ae19, "__brelse" },
	{ 0xa59da3c0, "down_write" },
	{ 0xa59da3c0, "up_write" },
	{ 0x2e24a63e, "set_nlink" },
	{ 0x680628e7, "ktime_get_real_ts64" },
	{ 0x885eccdf, "__bread_gfp" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x092a35a2, "_copy_to_user" },
	{ 0x888b8f57, "strcmp" },
	{ 0x5657e2ef, "d_add" },
	{ 0xb2fda715, "mount_bdev" },
	{ 0x8295115f, "lockref_get" },
	{ 0xf360faf3, "inode_set_ctime_to_ts" },
	{ 0x86c49e96, "nop_mnt_idmap" },
	{ 0xba157484, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x8471422e,
	0xa5d2285c,
	0xa61fd7aa,
	0x092a35a2,
	0xb03fd70b,
	0x692d8520,
	0x75f272a9,
	0xd36b6cbe,
	0xb03fd70b,
	0xd272d446,
	0x5a844b26,
	0x9174f503,
	0xd36b6cbe,
	0xe8213e80,
	0xd272d446,
	0x1dd0ae19,
	0xa59da3c0,
	0xa59da3c0,
	0x2e24a63e,
	0x680628e7,
	0x885eccdf,
	0xd272d446,
	0x092a35a2,
	0x888b8f57,
	0x5657e2ef,
	0xb2fda715,
	0x8295115f,
	0xf360faf3,
	0x86c49e96,
	0xba157484,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"inode_init_owner\0"
	"iget_locked\0"
	"__check_object_size\0"
	"_copy_from_user\0"
	"unregister_filesystem\0"
	"d_make_root\0"
	"write_dirty_buffer\0"
	"iput\0"
	"register_filesystem\0"
	"__fentry__\0"
	"__x86_indirect_thunk_rax\0"
	"kill_block_super\0"
	"unlock_new_inode\0"
	"_printk\0"
	"__stack_chk_fail\0"
	"__brelse\0"
	"down_write\0"
	"up_write\0"
	"set_nlink\0"
	"ktime_get_real_ts64\0"
	"__bread_gfp\0"
	"__x86_return_thunk\0"
	"_copy_to_user\0"
	"strcmp\0"
	"d_add\0"
	"mount_bdev\0"
	"lockref_get\0"
	"inode_set_ctime_to_ts\0"
	"nop_mnt_idmap\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "71F11905D47F1D7523D9869");
