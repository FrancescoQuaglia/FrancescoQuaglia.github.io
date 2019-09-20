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
	{ 0x4302d0eb, __VMLINUX_SYMBOL_STR(free_pages) },
	{ 0x216cbeb2, __VMLINUX_SYMBOL_STR(pv_cpu_ops) },
	{ 0x93fca811, __VMLINUX_SYMBOL_STR(__get_free_pages) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xdcb0349b, __VMLINUX_SYMBOL_STR(sys_close) },
	{ 0x7a2af7b4, __VMLINUX_SYMBOL_STR(cpu_number) },
	{ 0x7628f3c7, __VMLINUX_SYMBOL_STR(this_cpu_off) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "C7716E6E9FEC64C9A2BA03C");
