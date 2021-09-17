#define EXPORT_SYMTAB
#include <linux/module.h>
#include <linux/moduleparam.h>
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
#include <asm/page.h>
#include <asm/cacheflush.h>
#include <asm/apic.h>
#include <linux/syscalls.h>
#include "../include/monitor.h"

// This gives access to read_cr0() and write_cr0()
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,3,0)
    #include <asm/switch_to.h>
#else
    #include <asm/system.h>
#endif
#ifndef X86_CR0_WP
#define X86_CR0_WP 0x00010000
#endif


#define DEBUG if(0)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");
MODULE_DESCRIPTION("nesting of new system calls on the first entry originally reserved for sys_ni_syscall");
MODULE_DESCRIPTION("the new system calls implement a monitoring service via the stack area");

#define MODNAME "MONITOR"


int restore[2] = {[0 ... 1] -1};

unsigned long sys_call_table;// position of the syscall table - to be discovered at startup
unsigned long sys_ni_syscall_a;// position of the sys_ni_syscall code - to be discovred at startup


asmlinkage long sys_monitor_reset(void){
	
	monitor * p;

	printk("%s: sys_monitor_reset has been called - current is %d\n",MODNAME,current->pid);

	p = (void*)current->stack + sizeof(struct thread_info);

	p->cpu_id = -1;
	p->switches = 0;

	return 0;
}

asmlinkage long sys_get_cpu_id(monitor * buff){

	monitor * p;
	int ret;
	
	printk("%s: sys_get_cpu_id has been called - current is %d\n",MODNAME,current->pid);

	p = (void*)current->stack + sizeof(struct thread_info);

	printk("%s: cpuid info is:  %d\n",MODNAME,p->cpu_id);

       	ret = copy_to_user((char*)buff,(char*)p,sizeof(monitor));

	return 0;

}

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

	unsigned long * p;
	//unsigned long * aux;
	int i,j;
	unsigned long target;

	unsigned long cr0;

	printk("%s: initializing\n",MODNAME);

        p = sys_call_table = get_syscall_table();

        if(p == NULL){

                printk("%s: could not locate syscall table position\n",MODNAME);
                return -1;

        }

        for(i=0;i<256;i++){
                for(j=i+1;j<256;j++){
                        if(p[i] == p[j]){
                                target = p[i];
                                goto found;
                        }
                }

        }

        return -1;

found:

        sys_ni_syscall_a = target;


	j = -1;
	for (i=0; i<256; i++){
		if (p[i] == sys_ni_syscall_a){
			printk("%s: table entry %d keeps address %p\n",MODNAME,i,(void*)p[i]);
			j++;
			restore[j] = i;
			if (j == 1) break;
		}
	}

	cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);
	p[restore[0]] = (unsigned long)sys_monitor_reset;
	p[restore[1]] = (unsigned long)sys_get_cpu_id;
	write_cr0(cr0);

	printk("%s: new system-call sys_monitor_reset installed on sys-call table entry %d\n",MODNAME,restore[0]);
	printk("%s: new system-call sys_get_cpu_id installed on sys-call table entry %d\n",MODNAME,restore[1]);

	return  0;

}


void cleanup_module(void) {

	unsigned long * p = (unsigned long*) sys_call_table;
	unsigned long cr0;
        	
	printk("%s: shutting down\n",MODNAME);
	cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);
	p[restore[0]] = sys_ni_syscall_a;
	p[restore[1]] = sys_ni_syscall_a;
	write_cr0(cr0);
	printk("%s: sys-call table restored to its original content\n",MODNAME);
	
}

