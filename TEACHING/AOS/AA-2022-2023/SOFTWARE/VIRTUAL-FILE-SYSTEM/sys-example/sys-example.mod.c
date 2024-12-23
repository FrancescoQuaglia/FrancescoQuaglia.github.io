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
	{ 0x2f890937, __VMLINUX_SYMBOL_STR(param_ops_int) },
	{ 0x82596a47, __VMLINUX_SYMBOL_STR(kobject_put) },
	{ 0xda4985ce, __VMLINUX_SYMBOL_STR(sysfs_create_file_ns) },
	{ 0x8d3732d8, __VMLINUX_SYMBOL_STR(kobject_create_and_add) },
	{ 0x88463a86, __VMLINUX_SYMBOL_STR(kernel_kobj) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0x20c55ae0, __VMLINUX_SYMBOL_STR(sscanf) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "6319F1634CDF5625CBEB8AA");

MODULE_INFO(suserelease, "openSUSE Leap 15.0");
