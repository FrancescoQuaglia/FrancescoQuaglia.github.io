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

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xfe2565c8, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0x79bb2ea3, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0xc671e369, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0xda3e43d1, __VMLINUX_SYMBOL_STR(_raw_spin_unlock) },
	{ 0xd52bf1ce, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0x805f5e61, __VMLINUX_SYMBOL_STR(pid_task) },
	{ 0x6b82528a, __VMLINUX_SYMBOL_STR(find_vpid) },
	{ 0x1b17e06c, __VMLINUX_SYMBOL_STR(kstrtoll) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xb5419b40, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "487C777F95DE6C290EC3B6F");
