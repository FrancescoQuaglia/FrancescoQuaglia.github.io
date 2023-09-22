#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x881e648f, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x555f6938, __VMLINUX_SYMBOL_STR(lockref_get) },
	{ 0x4aa8777e, __VMLINUX_SYMBOL_STR(mount_bdev) },
	{ 0x87489c2d, __VMLINUX_SYMBOL_STR(d_add) },
	{ 0xb44ad4b3, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0x85a140a9, __VMLINUX_SYMBOL_STR(__bread_gfp) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x6cd6b1a5, __VMLINUX_SYMBOL_STR(set_nlink) },
	{ 0xb953351, __VMLINUX_SYMBOL_STR(__brelse) },
	{ 0x1628f40f, __VMLINUX_SYMBOL_STR(unlock_new_inode) },
	{ 0xe0d1ea3, __VMLINUX_SYMBOL_STR(kill_block_super) },
	{ 0x2ea2c95c, __VMLINUX_SYMBOL_STR(__x86_indirect_thunk_rax) },
	{ 0x211f68f1, __VMLINUX_SYMBOL_STR(getnstimeofday64) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0xb1a332f6, __VMLINUX_SYMBOL_STR(register_filesystem) },
	{ 0x4f9cde67, __VMLINUX_SYMBOL_STR(iput) },
	{ 0xed80e683, __VMLINUX_SYMBOL_STR(d_make_root) },
	{ 0x9d097cbf, __VMLINUX_SYMBOL_STR(unregister_filesystem) },
	{ 0xf1af6f64, __VMLINUX_SYMBOL_STR(iget_locked) },
	{ 0x9fd3fd86, __VMLINUX_SYMBOL_STR(inode_init_owner) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "C5D25F996C795F048AD38DF");

MODULE_INFO(suserelease, "openSUSE Leap 15.0");
