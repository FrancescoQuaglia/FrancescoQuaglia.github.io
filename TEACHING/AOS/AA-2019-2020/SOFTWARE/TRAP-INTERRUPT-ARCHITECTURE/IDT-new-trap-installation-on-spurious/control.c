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


#include "control.h"


unsigned my_trap_vector = 0xffU;//unsigned notation for 255


unsigned long audit_counter __attribute__((aligned(64)));
module_param(audit_counter, ulong, S_IRUSR | S_IRGRP | S_IROTH);


gate_desc old_entry;


void my_handler(struct pt_regs *regs){
	get_cpu();
	atomic_inc((atomic_t*)&audit_counter);
	printk("%s: I'm here\n",MOD_NAME);
	put_cpu();
}


#define HML_TO_ADDR(h,m,l)      ((unsigned long) (l) | ((unsigned long) (m) << 16) | ((unsigned long) (h) << 32))

static inline unsigned long get_address_entry_idt(unsigned int vector)
{
    struct desc_ptr idtr;
    gate_desc *gate_ptr;

    store_idt(&idtr);

    gate_ptr = (gate_desc *) (idtr.address + vector * sizeof(gate_desc));

    return (unsigned long) HML_TO_ADDR(gate_ptr->offset_high, gate_ptr->offset_middle, gate_ptr->offset_low);
}

static inline int get_address_from_symbol(unsigned long *address, const char *symbol)
{
    int ret;
    struct kprobe kp = {};

    kp.symbol_name = symbol;

    ret = register_kprobe(&kp);

    if (ret < 0) {
        printk("%s: symbol %s not found - returned value %d.\n", MOD_NAME, symbol, ret);
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

int setup_my_irq(void)
{
	int i;
	long address;
	unsigned int operand;
	unsigned long smp_spurious_address;
        unsigned long expected_smp_spurious_address;
        unsigned long cr0;
	struct desc_ptr idtr;
        unsigned char *byte;

	audit_counter = 0;

	/* read the idtr register */
        store_idt(&idtr);
	
	printk("%s: setting up the IRQ entry\n",MOD_NAME);

        if (!(address = get_address_entry_idt(target_vector)))
                return -ENODATA;
	printk("%s: address at entry 255 is %p\n",MOD_NAME,(void*)address);

        if (get_address_from_symbol(&smp_spurious_address, "smp_spurious_interrupt"))
                return -ENODATA;
	printk("%s: smp_spurious_interrupt address is %p\n",MOD_NAME,(void*)smp_spurious_address);

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

			printk("%s: call/displacement found\n",MOD_NAME);

                        operand = ((unsigned int) byte[i+1]) | (((unsigned int) byte[i+2]) << 8) |
                                (((unsigned int) byte[i+3]) << 16) | (((unsigned int) byte[i+4]) << 24);

                        expected_smp_spurious_address = (unsigned long) (((long) &byte[i+5]) + (long) operand);

                        if (expected_smp_spurious_address == smp_spurious_address) {

				printk("%s: match found!!!! ... hot patching\n",MOD_NAME);
                                old_call_operand = operand;
                                new_call_operand = (unsigned int) ((long) my_handler - ((long) &byte[i+5]));
                                call_operand_address = (unsigned int *) &byte[i+1];

                                cr0 = read_cr0();
                                write_cr0(cr0 & ~X86_CR0_WP);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
                               arch_cmpxchg(call_operand_address, old_call_operand, new_call_operand);
#else
                                cmpxchg(call_operand_address, old_call_operand, new_call_operand);
#endif

				write_idt_entry((gate_desc*)idtr.address, target_vector, &my_trap_desc);
                                write_cr0(cr0);

                                return 0;
                        }
                }
        }

	return -1;

}


void cleanup_my_irq(void) {
	unsigned long cr0;
	struct desc_ptr idtr;

	/* read the idtr register */
        store_idt(&idtr);

        cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
       arch_cmpxchg(call_operand_address, new_call_operand, old_call_operand);
#else
        cmpxchg(call_operand_address, new_call_operand, old_call_operand);
#endif

	write_idt_entry((gate_desc*)idtr.address, target_vector, &old_entry);
        write_cr0(cr0);

}

