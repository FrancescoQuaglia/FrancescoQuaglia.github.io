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
	{ 0x67c6a176, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xda3e43d1, __VMLINUX_SYMBOL_STR(_raw_spin_unlock) },
	{ 0x4629334c, __VMLINUX_SYMBOL_STR(__preempt_count) },
	{ 0x955a832f, __VMLINUX_SYMBOL_STR(___preempt_schedule) },
	{ 0x74deb10c, __VMLINUX_SYMBOL_STR(used_vectors) },
	{ 0xbd1763fe, __VMLINUX_SYMBOL_STR(current_task) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xd62c833f, __VMLINUX_SYMBOL_STR(schedule_timeout) },
	{ 0x216cbeb2, __VMLINUX_SYMBOL_STR(pv_cpu_ops) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0x7ff88cb6, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xd52bf1ce, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0x34f22f94, __VMLINUX_SYMBOL_STR(prepare_to_wait_event) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xfa66f77c, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0xdcb0349b, __VMLINUX_SYMBOL_STR(sys_close) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "CB8FF6C6DA963BBC85FBEE1");
