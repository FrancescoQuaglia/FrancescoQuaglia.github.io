#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <asm/desc.h>
#include <linux/printk.h>
#include<linux/slab.h>
#include<linux/kprobes.h>
#include <linux/binfmts.h>

#include "idt_entry_discovery.h"

MODULE_LICENSE("GPL");

#define HML_TO_ADDR(h,m,l)      ((unsigned long) (l) | ((unsigned long) (m) << 16) | ((unsigned long) (h) << 32))

//prototypes
void my_handler(struct pt_regs *, int);
int patch_IDT(unsigned long);


unsigned long audit_counter __attribute__((aligned(64)));
unsigned long cr0;

static unsigned int old_call_operand      = 0x0;
static unsigned int new_call_operand      = 0x0;
static unsigned int *call_operand_address = NULL;

gate_desc old_entry;
gate_desc my_trap_desc;


void my_handler(struct pt_regs *regs, int error_code) {
    printk("%s: The executable name --> %s\n", MOD_NAME, current->comm);
    printk("%s: RIP      --> %px\n", MOD_NAME, (void *)regs->ip);
    printk("%s: RSP user --> %px\n", MOD_NAME, (void *)regs->sp);
}

unsigned long cr0, cr4;

static inline void write_cr0_forced(unsigned long val){
        unsigned long __force_order;
        asm volatile("mov %0, %%cr0" : "+r"(val), "+m"(__force_order));
}

static inline void _protect_memory(void){
        write_cr0_forced(cr0);
}

static inline void _unprotect_memory(void){
        write_cr0_forced(cr0 & ~X86_CR0_WP);
}

static inline void write_cr4_forced(unsigned long val){
        unsigned long __force_order;
        asm volatile("mov %0, %%cr4" : "+r"(val), "+m"(__force_order));
}

static inline void conditional_cet_disable(void){
#ifdef X86_CR4_CET
        if (cr4 & X86_CR4_CET)
                write_cr4_forced(cr4 & ~X86_CR4_CET);
#endif
}

static inline void conditional_cet_enable(void){
#ifdef X86_CR4_CET
        if (cr4 & X86_CR4_CET)
                write_cr4_forced(cr4);
#endif
}

static inline void unprotect_memory(void){
        preempt_disable();
        cr0 = read_cr0();
        cr4 = native_read_cr4();
        conditional_cet_disable();
        _unprotect_memory();
}

static inline void protect_memory(void){
        _protect_memory();
        conditional_cet_enable();
        preempt_enable();
}



static unsigned long get_full_offset_by_vector(gate_desc *idt, int vector_number) {
    gate_desc *gate_ptr;
    gate_ptr = (gate_desc *)((unsigned long)idt + vector_number * sizeof(gate_desc));
    return (unsigned long)HML_TO_ADDR(gate_ptr->offset_high, gate_ptr->offset_middle, gate_ptr->offset_low);
}


static unsigned long get_full_offset_spurious_interrput(gate_desc *idt) {    
    unsigned long address;
    address = get_full_offset_by_vector(idt, SPURIOUS_APIC_VECTOR);
    return address;
}

/**
 *
 * this patch updates the second call instruction in the offset_spurious handler
 * which brings control to the actual event handler
 * in any case please check with the real structure of the kernel you are trying to mount this module on
 * see the recommendation inside this function 
 *
 */
int patch_IDT(unsigned long offset_spurious) {
    unsigned char *byte;
    unsigned int operand;
    unsigned long address;
    int is_first;
    int i;
    struct desc_ptr idtr;

    /* the first CALL does not need to be considered */
    is_first = 1;

    byte = (unsigned char *)offset_spurious;

    store_idt(&idtr);

    printk("%s: APPLYING THE PATCH\n",MOD_NAME);

    for(i=0; i<512; i++) {//this search is limited to a given number of bytes
        if(byte[i]==0xE8) {
            if(!is_first) {
                
                operand = ( (unsigned int) byte[i+1]       ) |
                          (((unsigned int) byte[i+2]) << 8 ) |
                          (((unsigned int) byte[i+3]) << 16) |
                          (((unsigned int) byte[i+4]) << 24);

                address = (unsigned long) (((long) &byte[i+5]) + (long) operand);

                //TODO: try to add a mechanism to check the actual correctness of the target of this call 
		//currently this module structure works on kernel 5/6

                old_call_operand = operand;
                new_call_operand = (unsigned int) ((long) my_handler - ((long) &byte[i+5]));
                call_operand_address = (unsigned int *) &byte[i+1];

           	unprotect_memory();
               	arch_cmpxchg(call_operand_address, old_call_operand, new_call_operand);
               	write_idt_entry((gate_desc*)idtr.address, SPURIOUS_APIC_VECTOR, &my_trap_desc);
               	protect_memory();

		return 0;
	}
        if(is_first) is_first = 0;                   
       }//end if
     }//end for

     return 1;

}




int idt_entry_discovery_init(void) {

    struct desc_ptr dtr;                                                    // Pointer to the IDT info
    int num_idt_entries;                                                    // Number of IDT entries
    unsigned long addr_spurious_first_handler;
    gate_desc *idt = NULL;                                                  // Pointer to the IDT table
    
    audit_counter = 0;

    //kernel 7 does not yet exist at this date but we suppose something might 
    //be changed in that kernel making it not compatible with this module
    if(LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0) || LINUX_VERSION_CODE >= KERNEL_VERSION(7,0,0)){
	printk("%s: unsupported kernel version\n",MOD_NAME);
	return -1;
    }	

    /* Get IDT info */
    store_idt(&dtr);
    idt = (gate_desc *)dtr.address;
    num_idt_entries = (dtr.size / sizeof(gate_desc)) + 1;

    addr_spurious_first_handler = get_full_offset_spurious_interrput(idt);

    //copy the old content of the target vector entry 
    memcpy(&old_entry, (void*)(dtr.address + SPURIOUS_APIC_VECTOR * sizeof(gate_desc)), sizeof(gate_desc));    
    //pack a new gate for access from user land 
    pack_gate(&my_trap_desc, GATE_TRAP, (unsigned long)addr_spurious_first_handler, 0x3, 0, 0);

    if (!patch_IDT(addr_spurious_first_handler)){
     	printk("%s: setup executed...\n", MOD_NAME);
    	return 0;
    }
     
     return -1;

}

void idt_entry_discovery_exit(void) {
	struct desc_ptr idtr;

	store_idt(&idtr);

	unprotect_memory();
	arch_cmpxchg(call_operand_address, new_call_operand, old_call_operand);
	write_idt_entry((gate_desc*)idtr.address, SPURIOUS_APIC_VECTOR, &old_entry);
	protect_memory();

   	printk("%s: exit...\n", MOD_NAME);
}

module_init(idt_entry_discovery_init);
module_exit(idt_entry_discovery_exit);

