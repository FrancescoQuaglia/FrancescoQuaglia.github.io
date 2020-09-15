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



#define MAX_TICKS (1<<30) //this is just a reference value for the live demo

unsigned context_switch_vector = 0xffU;//unsigned notation for 255

gate_desc old_entry;


elem head = {NULL,-1,NULL,NULL};
elem tail = {NULL,-1,NULL,NULL};
elem* last_running = NULL;
spinlock_t queue_lock; 


asmlinkage long thread_manager_reset(void){

	elem* aux;

	last_running = NULL;

        spin_lock(&queue_lock);
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
        spin_unlock(&queue_lock);

	return 0;
}

asmlinkage long new_thread(unsigned int ip, unsigned int stack){
        
	elem *p, *aux;;

	
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

        preempt_disable();//redundant
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
        preempt_enable();//redundant

	printk("%s: new thread context linked to the contexts queue\n",MODNAME);

	return 0;
}



void context_switch_handler(struct pt_regs *regs){

	elem* aux;
	elem* next_task;
	long int ticks;

	aux = &head;

        preempt_disable();//redundant
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
        preempt_enable();//redundant

	return;
	
}

void context_switch(void);
asm("    .globl context_switch\n"
    "context_switch:\n"
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
	gate_desc context_switch_desc;
	unsigned long cr0;

	/* read the idtr register */
	store_idt(&idtr);

	/* copy the old entry before overwritting it */
	memcpy(&old_entry, (void*)(idtr.address + context_switch_vector * sizeof(gate_desc)), sizeof(gate_desc));
	
	pack_gate(&context_switch_desc, GATE_INTERRUPT, (unsigned long)context_switch, 0x3, 0, 0);
	
	/* the IDT id read only */
	cr0 = read_cr0();
	write_cr0(cr0 & ~X86_CR0_WP);

	write_idt_entry((gate_desc*)idtr.address, context_switch_vector, &context_switch_desc);
	
	/* restore the Write Protection BIT */
	write_cr0(cr0);	

	return 0;
}// setup_idt

static int acquire_free_vector(void)
{
	while (test_bit(context_switch_vector, used_vectors)) {
		if (context_switch_vector == 0x40) {//below they are reserved
			printk("%s: No free vector found\n",MODNAME);
			return -1;
		}
		context_switch_vector--;
	}
	set_bit(context_switch_vector, used_vectors);
	printk("%s: Got vector #%x - displacement %u\n",MODNAME, context_switch_vector,context_switch_vector);
	return 0;
}

int setup_my_irq(void)
{
	int err = 0;
	
	printk("%s: setting up lwt IRQ\n",MODNAME);

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

	write_idt_entry((gate_desc*)idtr.address, context_switch_vector, &old_entry);
	
	/* restore the Write Protection BIT */
	write_cr0(cr0);	
}// restore_idt

void cleanup_my_irq(void)
{

	/* release the lwt vector */
	restore_idt();

	clear_bit(context_switch_vector, used_vectors);	

	pr_info("%s: IDT and vector bitmap restored\n", MODNAME);
}

