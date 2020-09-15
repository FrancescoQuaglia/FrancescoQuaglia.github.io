#include <linux/kernel.h>
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

#include "control.h"


unsigned my_trap_vector = 0xffU;//unsigned notation for 255 - let's try to find a free IDT entry starting from this vector value


unsigned long audit_counter __attribute__((aligned(64)));//we can count the number of times this handler has been fired
module_param(audit_counter, ulong, S_IRUSR | S_IRGRP | S_IROTH);

gate_desc old_entry;

void my_handler(struct pt_regs *regs){
	atomic_inc((atomic_t*)&audit_counter);
}

void my_trap_entry(void);
asm("    .globl my_trap_entry\n"
    "my_trap_entry:\n"
    "    testq $3,8(%rsp)\n"
    "    jz    1f\n"//Am I already kernel mode?
    "    swapgs\n"
    "1:\n"
    "    pushq $0\n" /* error code */
    "    pushq %rdi\n"
    "    pushq %rsi\n"
    "    pushq %rdx\n"
    "    pushq %rcx\n"
    "    pushq %rax\n"
    "    pushq %r8\n"
    "    pushq %r9\n"
    "    pushq %r10\n"
    "    pushq %r11\n"
    "    pushq %rbx\n"
    "    pushq %rbp\n"
    "    pushq %r12\n"
    "    pushq %r13\n"
    "    pushq %r14\n"
    "    pushq %r15\n"
    "    mov %rsp,%rdi\n"
    "1:  call my_handler\n"
    "    popq %r15\n"
    "    popq %r14\n"
    "    popq %r13\n"
    "    popq %r12\n"
    "    popq %rbp\n"
    "    popq %rbx\n"
    "    popq %r11\n"
    "    popq %r10\n"
    "    popq %r9\n"
    "    popq %r8\n"
    "    popq %rax\n"
    "    popq %rcx\n"
    "    popq %rdx\n"
    "    popq %rsi\n"
    "    popq %rdi\n"
    "    addq $8,%rsp\n" /* error code removal */
    "    testq $3,8(%rsp)\n"
    "    jz 2f\n"
    "    swapgs\n"
    "2:\n"
    "    iretq");


static int setup_idt_entry (void) {
	
	struct desc_ptr idtr;
	gate_desc my_trap_desc;
	unsigned long cr0;

	/* read the idtr register */
	store_idt(&idtr);

	/* copy the old entry before overwritting it */
	memcpy(&old_entry, (void*)(idtr.address + my_trap_vector * sizeof(gate_desc)), sizeof(gate_desc));
	
	pack_gate(&my_trap_desc, GATE_INTERRUPT, (unsigned long)my_trap_entry, 0x3, 0, 0);
	
	/* the IDT id read only */
	cr0 = read_cr0();
	write_cr0(cr0 & ~X86_CR0_WP);

	write_idt_entry((gate_desc*)idtr.address, my_trap_vector, &my_trap_desc);
	
	/* restore the Write Protection BIT */
	write_cr0(cr0);	

	return 0;
}// setup_idt

static int acquire_free_vector(void) {

	while (test_bit(my_trap_vector, used_vectors)) {
		if (my_trap_vector == 0x40) {//below they are reserved
			printk("%s: no free vector found\n",MOD_NAME);
			return -1;
		}
		my_trap_vector--;
	}

	set_bit(my_trap_vector, used_vectors);
	printk("%s: got vector #%x - displacement %u\n",MOD_NAME, my_trap_vector,my_trap_vector);
	return 0;
}

int setup_my_irq(void) {

	int err = 0;
	
	printk("%s: setting up ibs IRQ\n",MOD_NAME);

	audit_counter = 0;

	err = acquire_free_vector();
	if (err) goto out;

	err = setup_idt_entry();
	if (err) goto out;
	
out:
	return err;
}

static void restore_idt(void) {
	struct desc_ptr idtr;
	unsigned long cr0;

	/* read the idtr register */
	store_idt(&idtr);

	/* the IDT id read only */
	cr0 = read_cr0();
	write_cr0(cr0 & ~X86_CR0_WP);

	write_idt_entry((gate_desc*)idtr.address, my_trap_vector, &old_entry);
	
	/* restore the Write Protection BIT */
	write_cr0(cr0);	
}// restore_idt

void cleanup_my_irq(void) {

	/* release the trap vector */
	restore_idt();

	clear_bit(my_trap_vector, used_vectors);	
	
	pr_info("%s: IDT restored\n",MOD_NAME);
}

