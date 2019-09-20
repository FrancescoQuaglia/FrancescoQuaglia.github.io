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


#define AUDIT if(1) 

#define NO (0)
#define YES (NO+1)
#define MAX_TICKS (1<<30)

unsigned ibs_vector = 0xffU;//unsigned notation for 255

gate_desc old_entry;


elem head = {NULL,-1,-1,NULL,NULL};
elem tail = {NULL,-1,-1,NULL,NULL};
elem* last_running = NULL;
spinlock_t queue_lock; 


asmlinkage long thread_manager_reset(void){

	elem* aux;

	last_running = NULL;

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

	return 0;
}

asmlinkage long new_thread(unsigned int ip, unsigned int stack){
        
	elem *p, *aux;;

	
	p = (elem*)kzalloc(sizeof(elem),GFP_KERNEL);
	if(p == NULL){
		AUDIT
		printk("%s: no more contexts thread structures\n",MODNAME);
		return -1;
	}

	p->context = kzalloc(sizeof(struct pt_regs),GFP_KERNEL);
	if(p->context == NULL){
		AUDIT
		printk("%s: no more thread contexts\n",MODNAME);
		kfree(p);
		return -1;
	}

	p->context->ip = ip;//rip setup for user pace return
	p->context->sp = stack;//rsp setup for user space return
	p->context->ss = 0x2bU;//rsp setup for user space return
	p->context->cs = 0x33U;//cs setup for user space return
	p->context->flags = 0x200U;//flags setup for user space return
	p->ticks = 0;

        AUDIT
        printk("%s: new thread context setup\n",MODNAME);

        preempt_disable();
        spin_lock(&queue_lock);

	aux = &tail;
        if(aux->prev == NULL){
                spin_unlock(&queue_lock);
                preempt_enable();
                printk("%s: malformed thread queue\n",MODNAME);
                return -1;
        }

        aux->prev->next = p;
        p->prev = aux->prev;
        aux->prev = p;
        p->next = aux;

        spin_unlock(&queue_lock);
        preempt_enable();

	AUDIT
	printk("%s: new thread linked to the thread queue\n",MODNAME);

	return 0;
}



void context_switch_handler(struct pt_regs *regs){

	elem* aux;
	elem* next_task;
	long int ticks;

	aux = &head;

        preempt_disable();
        spin_lock(&queue_lock);

        if(aux == NULL){
                spin_unlock(&queue_lock);
                preempt_enable();
                printk("%s: malformed thread queue\n",MODNAME);
                return;
        }

	if(last_running != NULL){
		memcpy((void*)(last_running->context),(void*)regs,sizeof(struct pt_regs));
	}

	next_task = NULL;
	ticks = MAX_TICKS;
	while(aux->next != &tail){
                printk("%s: queue scanning\n",MODNAME);
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
	AUDIT
        printk("%s: scheduling complete - restoring CPU context\n",MODNAME);
	memcpy((void*)regs,(void*)(last_running->context),sizeof(struct pt_regs));

        spin_unlock(&queue_lock);
        preempt_enable();

	return;
	
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
    "1:  call context_switch_handler\n"
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
			printk("%s: No free vector found\n",MODNAME);
			return -1;
		}
		ibs_vector--;
	}
	set_bit(ibs_vector, used_vectors);
	printk("%s: Got vector #%x - displacement %u\n",MODNAME, ibs_vector,ibs_vector);
	return 0;
}

int setup_my_irq(void)
{
	int err = 0;
	
	printk("%s: setting up ibs IRQ\n",MODNAME);

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

	pr_info("%s: IRQ cleaned\n", MODNAME);
}

