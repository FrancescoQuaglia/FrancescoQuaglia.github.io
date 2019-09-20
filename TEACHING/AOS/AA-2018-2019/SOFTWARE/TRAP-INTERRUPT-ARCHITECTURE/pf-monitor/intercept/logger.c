#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <asm/uaccess.h>
#include "traps-dummy.h"
#include <asm/desc_defs.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include "log.h"

#define MASK (0x02)
#define AUDIT if(0)

static inline unsigned long my_native_read_cr2(void)
{
         unsigned long val;
         asm volatile("mov %%cr2,%0\n\t" : "=r" (val), "=m" (__force_order));
         return val;
}

void page_fault_monitor(struct pt_regs * regs, unsigned long error_code, long flags){
	unsigned long addr;
	pf_log * p;

	
	addr = my_native_read_cr2();

	p = (void*)current->stack + sizeof(struct thread_info);
	
	AUDIT
 	printk("thread %d - faulting at address %p - error code is %lu\n",current->pid,(void*)addr,error_code);

	error_code &= MASK;

	switch (error_code){
	 	case 0:
			AUDIT
 			printk("thread %d - faulting at address %p - masked error code is %lu\n",current->pid,(void*)addr,error_code);
			p->read++;
		 break;

		case 2:
			AUDIT
 			printk("thread %d - faulting at address %p - masked error code is %lu\n",current->pid,(void*)addr,error_code);
			p->write++;
		 break;

		default: ; 
	}
}
