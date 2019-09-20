#define EXPORT_SYMTAB
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/kprobes.h>
#include <linux/mutex.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/syscalls.h>
#include <asm/page.h>
#include <asm/cacheflush.h>
#include <asm/apic.h>

// This gives access to read_cr0() and write_cr0()
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,3,0)
    #include <asm/switch_to.h>
#else
    include <asm/system.h>
#endif
#ifndef X86_CR0_WP
#define X86_CR0_WP 0x00010000
#endif


#define DEBUG if(0)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <quaglia@dis.uniroma1.it>");
MODULE_DESCRIPTION("runtime discovery of the positioning of the syscall-table");

#define MODNAME "SYS-CALL TABLE DICOVER"


/* this value is taken from system map */
unsigned long sys_call_table = 0xffffffff81800300;


unsigned long *get_syscall_table(void)
{
        unsigned long *syscall_table;
        unsigned long int i;

        for (i = (unsigned long int)sys_close; i < ULONG_MAX; i += sizeof(void *)) {
                syscall_table = (unsigned long *)i;

                if (syscall_table[__NR_close] == (unsigned long)sys_close)
                        return syscall_table;
        }
        return NULL;
}


int init_module(void) {

	unsigned long * p = (unsigned long *) sys_call_table;
	unsigned long * aux;

	aux = get_syscall_table();

	printk("discovery is %p - statically known from system map is %p\n",aux,p);

	return 0;

}


void cleanup_module(void) {

	return;
	
}

