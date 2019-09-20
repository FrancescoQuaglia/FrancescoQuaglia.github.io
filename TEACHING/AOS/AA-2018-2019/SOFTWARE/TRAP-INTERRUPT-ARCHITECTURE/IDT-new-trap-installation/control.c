#include <asm/apicdef.h>
#include <asm/apic.h>
#include <asm/nmi.h>
#include <linux/slab.h>
#include <asm/desc.h>
#include <asm/desc_defs.h>

#include <asm/irq.h>		// NR_IRQS
#include <asm/hw_irq.h>

#include <linux/interrupt.h>	// set_irq_regs


#include "control.h"


unsigned ibs_vector = 0xffU;//unsigned notation for 255


gate_desc old_entry;


void my_handler(struct pt_regs *regs){
	printk("you have trapped here r15 is %lu - restoring on RIP\n",regs->r15);
	regs->ip = regs->r15;
}

extern void ibs_entry(void);
asm("    .globl ibs_entry\n"
    "ibs_entry:\n"
    "    testq $3,8(%rsp)\n"
    "    jz    1f\n"
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
    "    addq $8,%rsp\n" /* error code */
    "    testq $3,8(%rsp)\n"
    "    jz 2f\n"
    "    swapgs\n"
    "2:\n"
    "    iretq");

static int setup_idt_entry (void)
{
	
	struct desc_ptr idtr;
	gate_desc ibs_desc;
	unsigned long cr0;

	/* read the idtr register */
	store_idt(&idtr);

	/* copy the old entry before overwritting it */
	memcpy(&old_entry, (void*)(idtr.address + ibs_vector * sizeof(gate_desc)), sizeof(gate_desc));
	
	pack_gate(&ibs_desc, GATE_TRAP, (unsigned long)ibs_entry, 0x3, 0, 0);
	
	/* the IDT id read only */
	cr0 = read_cr0();
	write_cr0(cr0 & ~X86_CR0_WP);

	write_idt_entry((gate_desc*)idtr.address, ibs_vector, &ibs_desc);
	
	/* restore the Write Protection BIT */
	write_cr0(cr0);	

	return 0;
}// setup_idt

static int acquire_free_vector(void)
{
	while (test_bit(ibs_vector, used_vectors)) {
		if (ibs_vector == 0x40) {//below they are reserved
			printk("No free vector found\n");
			return -1;
		}
		ibs_vector--;
	}
	set_bit(ibs_vector, used_vectors);
	printk("Got vector #%x - displacement %u\n", ibs_vector,ibs_vector);
	return 0;
}

int setup_my_irq(void)
{
	int err = 0;
	
	printk("setting up ibs IRQ\n");

	err = acquire_free_vector();
	if (err) goto out;

	err = setup_idt_entry();
	if (err) goto out;
	
out:
	return err;
}

static void restore_idt(void)
{
	struct desc_ptr idtr;
	unsigned long cr0;

	/* read the idtr register */
	store_idt(&idtr);

	/* the IDT id read only */
	cr0 = read_cr0();
	write_cr0(cr0 & ~X86_CR0_WP);

	write_idt_entry((gate_desc*)idtr.address, ibs_vector, &old_entry);
	
	/* restore the Write Protection BIT */
	write_cr0(cr0);	
}// restore_idt

void cleanup_my_irq(void)
{

	restore_idt();
	/* release the ibs vector */

	clear_bit(ibs_vector, used_vectors);	
	
	pr_info("IRQ cleaned\n");
}

