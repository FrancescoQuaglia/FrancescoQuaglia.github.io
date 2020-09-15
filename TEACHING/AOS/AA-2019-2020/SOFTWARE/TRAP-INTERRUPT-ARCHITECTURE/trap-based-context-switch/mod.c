#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kprobes.h>
#include <linux/syscalls.h>
#include <linux/slab.h>

#include "control.h"

extern void my_handler(void);
extern long thread_manager_reset(void);
extern long new_thread(unsigned int, unsigned int);

int restore[2] = {[0 ... 1] -1};

unsigned long sys_call_table;// position of the syscall table - to be discovered at startup
unsigned long sys_ni_syscall_a;// position of the sys_ni_syscall code - to be discovred at startup

extern elem head, tail;


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


static __init int my_irq_init(void)
{

	int err = 0;

	unsigned long * p;
        int i,j;
        unsigned long target;

        unsigned long cr0;

        printk("%s: initializing\n",MODNAME);
	
        head.next = &tail;// setup initial double linked list 
        tail.prev = &head;


	err = setup_my_irq();
	if (err) goto out;

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
        p[restore[0]] = (unsigned long)thread_manager_reset;
        p[restore[1]] = (unsigned long)new_thread;
        write_cr0(cr0);

        printk("%s: new system-call thread_manager_reset installed on sys-call table entry %d\n",MODNAME,restore[0]);
        printk("%s: new system-call new_thread installed on sys-call table entry %d\n",MODNAME,restore[1]);


out:
	return err;
}// my irq init

void __exit my_irq_exit(void) { 

	unsigned long * p = (unsigned long*) sys_call_table;
        unsigned long cr0;
	elem * aux;;
                
        printk("%s: shutting down\n",MODNAME);

        cr0 = read_cr0();
        write_cr0(cr0 & ~X86_CR0_WP);
        p[restore[0]] = sys_ni_syscall_a;
        p[restore[1]] = sys_ni_syscall_a;
        write_cr0(cr0);

	cleanup_my_irq();

	//now memory release
	aux = &head;
	while(aux->next != &tail){
		kfree(aux->next->context);
		aux = aux->next;
	//	kfree(aux->prev);
	}
        printk("%s: memory release preamble done\n",MODNAME);

	aux = &head;
	while(aux != &tail){
		aux=aux->next;
		if(aux->prev!=&head) kfree(aux->prev);
//		aux = aux->next;
	//	kfree(aux->prev);
	}

//	if(tail.prev != &head ){
		 //kfree(tail.prev->context);
//		 kfree(tail.prev);
//	}	

        printk("%s: sys-call table restored to its original content\n",MODNAME);

	pr_info("%s: Module exit\n",MODNAME);

}// module exit

// Register these functions
module_init(my_irq_init);
module_exit(my_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia");
