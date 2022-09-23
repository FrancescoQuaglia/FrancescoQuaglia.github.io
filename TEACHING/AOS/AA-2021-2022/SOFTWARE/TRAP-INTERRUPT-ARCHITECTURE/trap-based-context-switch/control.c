/*
* 
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License as published by the Free Software
* Foundation; either version 3 of the License, or (at your option) any later
* version.
* 
* This module is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
* A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* 
* @file control.c 
* @brief This is a source file for the Linux Kernel Module which implements
*       a trap-based context switch among new threads of a new kind  
*       the module does not account for vectorized registers in order to save/restore
*       the context of a thread
*
* @author Francesco Quaglia
*
* @date November 27, 2021
*/


#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/ktime.h>
#include <linux/limits.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <linux/ptrace.h>
#include <linux/smp.h>
#include <asm/apicdef.h>
#include <asm/apic.h>
#include <asm/nmi.h>
#include <linux/slab.h>
#include <asm/desc.h>
#include <asm/desc_defs.h>
#include <asm/irq.h>		
#include <asm/hw_irq.h>
#include <linux/interrupt.h>	
#include <linux/syscalls.h>	

#include "lib/include/scth.h"
#include "control.h"

#define MAX_TICKS (1<<30) //this is just a reference value for the live demo

unsigned my_trap_vector = 0xffU;//unsigned notation for 255


unsigned long audit_counter __attribute__((aligned(64)));
module_param(audit_counter, ulong, S_IRUSR | S_IRGRP | S_IROTH);


gate_desc old_entry;

elem head = {NULL,-1,NULL,NULL};
elem tail = {NULL,-1,NULL,NULL};
elem* last_running = NULL;
int the_pid;
module_param(the_pid, int, 0660);
spinlock_t queue_lock;


#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
__SYSCALL_DEFINEx(1, _thread_manager_reset, int, unused){
#else
asmlinkage long sys_thread_manager_reset(int unused){
#endif

        elem* aux;

        last_running = NULL;

        spin_lock(&queue_lock);
	if(the_pid != 0){
        	spin_unlock(&queue_lock);
		return -1;
	}
        aux = &head;
        while(aux->next != &tail){
                kfree(aux->next->context);
                aux = aux->next;
        }

        aux = &head;
        while(aux != &tail){
                aux=aux->next;
                if(aux->prev!=&head) kfree(aux->prev);
        }

        head.next = &tail;// reset initial double linked list
        tail.prev = &head;

	the_pid = current->pid;
        spin_unlock(&queue_lock);

        return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
__SYSCALL_DEFINEx(2, _new_thread, unsigned int, ip, unsigned int, stack){
#else
asmlinkage long sys_new_thread(unsigned int ip, unsigned int stack){
#endif

        elem *p, *aux;;

	if(current->pid != the_pid) return -1;

        p = (elem*)kzalloc(sizeof(elem),GFP_KERNEL);
        if(p == NULL){
                printk("%s: unable to allocate control block\n",MODNAME);
                return -1;
        }

        p->context = kzalloc(sizeof(struct pt_regs),GFP_KERNEL);
        if(p->context == NULL){
                printk("%s: unable to allocate context\n",MODNAME);
                kfree(p);
                return -1;
        }

        p->context->ip = ip;//rip setup for user pace return
        p->context->sp = stack;//rsp setup for user space return
        p->context->ss = 0x2bU;//ss setup for user space return
        p->context->cs = 0x33U;//cs setup for user space return
        p->context->flags = 0x200U;//flags setup for user space return
        p->ticks = 0;//we initially appear has having used no CPU time

        printk("%s: new thread context setup\n",MODNAME);

        spin_lock(&queue_lock);

        aux = &tail;
        if(aux->prev == NULL){
                spin_unlock(&queue_lock);
                preempt_enable();
                printk("%s: malformed contexts queue\n",MODNAME);
                return -1;
        }

        aux->prev->next = p;
        p->prev = aux->prev;
        aux->prev = p;
        p->next = aux;

        spin_unlock(&queue_lock);

        printk("%s: new thread context linked to the contexts queue\n",MODNAME);

        return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
long sys_thread_manager_reset = (unsigned long) __x64_sys_thread_manager_reset;
long sys_new_thread = (unsigned long) __x64_sys_new_thread;
#else
#endif


void context_switch_handler(struct pt_regs *regs){

        elem* aux;
        elem* next_task;
        long int ticks;

	if(current->pid != the_pid) return;

        aux = &head;

        spin_lock(&queue_lock);

        if(aux == NULL){
                spin_unlock(&queue_lock);
                preempt_enable();//redundant
                return;
        }

        if(last_running != NULL){
                memcpy((void*)(last_running->context),(void*)regs,sizeof(struct pt_regs));
        }

        next_task = NULL;
        ticks = MAX_TICKS;
        while(aux->next != &tail){
                if(aux->next->ticks < ticks){
                        next_task = aux->next;
                        ticks = aux->next->ticks;
                }       
                aux = aux->next;
        }

        if(next_task==NULL){
                spin_unlock(&queue_lock);
                preempt_enable();
                return;
        }

        last_running = next_task;
        last_running->ticks++;
        memcpy((void*)regs,(void*)(last_running->context),sizeof(struct pt_regs));

        spin_unlock(&queue_lock);

        return;
        
}


#define HML_TO_ADDR(h,m,l)      ((unsigned long) (l) | ((unsigned long) (m) << 16) | ((unsigned long) (h) << 32))

static inline unsigned long get_address_entry_idt(unsigned int vector) {

    struct desc_ptr idtr;
    gate_desc *gate_ptr;

    store_idt(&idtr);

    gate_ptr = (gate_desc *) (idtr.address + vector * sizeof(gate_desc));

    return (unsigned long) HML_TO_ADDR(gate_ptr->offset_high, gate_ptr->offset_middle, gate_ptr->offset_low);
}

static inline int get_address_from_symbol(unsigned long *address, const char *symbol) {

    int ret;
    struct kprobe kp = {};

    kp.symbol_name = symbol;

    ret = register_kprobe(&kp);

    if (ret < 0) {
	    //this may fail since the current kernel might no longer use/kprobe the target symbol 
        printk("%s: symbol %s not found - returned value %d.\n", MODNAME, symbol, ret);
        return ret;
    }

    *address = (unsigned long) kp.addr;
    unregister_kprobe(&kp);

    return 0;
}

static unsigned int old_call_operand = 0x0;
static unsigned int new_call_operand = 0x0;
static unsigned int *call_operand_address = NULL;

gate_desc old_entry;
gate_desc my_trap_desc;

int target_vector = 255;//hardcoded vector number for the spurious IRQ supposed entry

int setup_my_irq(void) {

	int i;
	long address;
	unsigned int operand;
	unsigned long smp_spurious_address;
        unsigned long expected_smp_spurious_address;
        //unsigned long cr0;
	struct desc_ptr idtr;
        unsigned char *byte;

	audit_counter = 0;

	/* read the idtr register */
        store_idt(&idtr);
	
	printk("%s: setting up the IRQ entry\n",MODNAME);

        if (!(address = get_address_entry_idt(target_vector)))
                return -ENODATA;
	printk("%s: address at entry 255 is %p\n",MODNAME,(void*)address);

        if (get_address_from_symbol(&smp_spurious_address, "smp_spurious_interrupt"))
                return -ENODATA;

	printk("%s: smp_spurious_interrupt address is %p\n",MODNAME,(void*)smp_spurious_address);

	/* read the idtr register */
        store_idt(&idtr);
	/* copy the old content of the target vector entry */
        memcpy(&old_entry, (void*)(idtr.address + target_vector * sizeof(gate_desc)), sizeof(gate_desc));

	/* pack a new gate for access from user land */
        pack_gate(&my_trap_desc, GATE_TRAP, (unsigned long)address, 0x3, 0, 0);


	//now the real hot patch
        byte = (unsigned char *) address;

	for (i=0; i<512; i++) {
                if (byte[i] == 0xE8) {

			printk("%s: call/displacement found\n",MODNAME);

                        operand = ((unsigned int) byte[i+1]) | (((unsigned int) byte[i+2]) << 8) |
                                (((unsigned int) byte[i+3]) << 16) | (((unsigned int) byte[i+4]) << 24);

                        expected_smp_spurious_address = (unsigned long) (((long) &byte[i+5]) + (long) operand);

                        if (expected_smp_spurious_address == smp_spurious_address) {

				printk("%s: match found!!!! ... hot patching\n",MODNAME);
                               	old_call_operand = operand;
                              	new_call_operand = (unsigned int) ((long) context_switch_handler - ((long) &byte[i+5]));
                              	call_operand_address = (unsigned int *) &byte[i+1];
			 
			      	unprotect_memory();

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
                               	arch_cmpxchg(call_operand_address, old_call_operand, new_call_operand);
#else
                               	cmpxchg(call_operand_address, old_call_operand, new_call_operand);
#endif
                               	write_idt_entry((gate_desc*)idtr.address, target_vector, &my_trap_desc);

			       	protect_memory();

                                return 0;
                        }
                }
        }

	return -1;

}


void cleanup_my_irq(void) {

	struct desc_ptr idtr;

	// read the idtr register
        store_idt(&idtr);

	unprotect_memory();

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
 	arch_cmpxchg(call_operand_address, new_call_operand, old_call_operand);
#else
        cmpxchg(call_operand_address, new_call_operand, old_call_operand);
#endif

	protect_memory();

}

