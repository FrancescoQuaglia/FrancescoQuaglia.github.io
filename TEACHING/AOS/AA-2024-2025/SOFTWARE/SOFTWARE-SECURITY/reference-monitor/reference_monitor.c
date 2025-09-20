/**		      Copyright (C) 2014-2015 HPDCS Group
*		       http://www.dis.uniroma1.it/~hpdcs
* 
* This is free software; 
* You can redistribute it and/or modify this file under the
* terms of the GNU General Public License as published by the Free Software
* Foundation; either version 3 of the License, or (at your option) any later
* version.
* 
* This file is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
* A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* 
* @brief This is a simple Linux Kernel Module which implements
*	 a mandatory policy for the execve service, closing it to the root user for a 
*	 black list of executables
*
* @author Francesco Quaglia
*
* @date December 19, 2018
* @update December 14, 2024
*
*/

#include <linux/module.h>
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

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 17, 0)
#define target_func "sys_execve" //you should modify this depending on the kernel version
#else
#define target_func "__x64_sys_execve" //you should modify this depending on the kernel version
#endif

#define AUDIT if(1)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");
MODULE_DESCRIPTION("see the README file");

#define MODNAME "Reference monitor"

#define UNLOADABLE

char *black_list[] = {"bash","tcsh","sh","sudo", NULL ,"su", NULL}; //the list of programs root cannot use to spawn others - being root is still admitted excluding 'su' from the list 

//#define STRONGER //this allows you to blacklist the above executables even for EUID-root users

#ifdef STRONGER
#define	corrector (current->cred->euid.val == 0)
#else
#define	corrector (0)
#endif

//where to look at when searching system call parameters
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
#define get(regs)       regs = (struct pt_regs*)the_regs->di;
#else
#define get(regs)       regs = the_regs;
#endif

#define MAX_LEN 256

DEFINE_PER_CPU(unsigned long *, kprobe_context_pointer);//this is used for steady state operations 

static struct kretprobe setup_probe;  
static struct kretprobe retprobe;  
static struct kretprobe *the_retprobe = &setup_probe;  

static int sys_execve_wrapper(struct kretprobe_instance *ri, struct pt_regs *the_regs){

        struct pt_regs * regs;
	char command[MAX_LEN];
	int i=-1;
	int ret = 0;
        unsigned long* kprobe_cpu;

	
	get(regs);

        kprobe_cpu = __this_cpu_read(kprobe_context_pointer);
        __this_cpu_write(*kprobe_cpu, NULL);
        preempt_enable();

	do{
	    ++i;
	    ret = copy_from_user(&(command[i]),&(((char*)(regs->di))[i]),1);
	}while(i<(MAX_LEN-1) && (command[i] != '\n')&&(ret==0));
	command[i]='\0';

        preempt_disable();
        kprobe_cpu = __this_cpu_read(kprobe_context_pointer);
#ifndef CONFIG_KRETPROBE_ON_RETHOOK
        __this_cpu_write(*kprobe_cpu, &ri->rp->kp);
#else
        __this_cpu_write(*kprobe_cpu, &the_retprobe->kp);
#endif

	printk("%s: --------------------------------------------\n",MODNAME);
	printk("%s: this is the wrapper for execve - trying to run: %s\n",MODNAME,command);
	printk("%s: request on behalf of user %d - euid %d (current command is: %s)\n",MODNAME,current->cred->uid.val,current->cred->euid.val,command);

	if(strcmp(command,"/usr/bin/dmesg")==0){
		printk("%s: accepting dmesg just for audit purpose\n",MODNAME);
		return 1;
	}

	if(current->cred->uid.val == 0 || corrector){
		printk("%s: need black list search\n",MODNAME);
		for(i=0; black_list[i] != NULL; i++){
			if(strcmp(black_list[i],current->comm) == 0 ){
				AUDIT
				printk("%s: current couple <program,UID> is black listed in the UID domain - execve rejected\n",MODNAME);//the domain can include EUID specification depedning in the value of 'corrector'
				goto reject;
			}
		}
		AUDIT
		printk("%s: current couple <program,UID> can run %s according to domain specification - finally executing the requested execve \n",MODNAME,command);
		
	}	

	return 1;

reject:
	regs->di = (unsigned long)NULL;
	return 0;

}

static int return_value(struct kretprobe_instance *ri, struct pt_regs *the_regs) { 

	the_regs->ax = -EPERM;
	return 0;
}


//the function to hook at startup for setting up per-cpu variables via kprobe
#define setup_target_func "run_on_cpu"


void run_on_cpu(void* x){//this is here just to enable a kprobe on it 
        printk("%s: reference monitor setup - running on CPU %d\n", MODNAME, smp_processor_id());
        return;
}

unsigned long successful_search_counter = 0;//number of CPUs that succesfully found the address of the per-CPU variable that keeps the reference to the current kprobe context 

DEFINE_PER_CPU(unsigned long, BRUTE_START);//redundant you might use the below per-cpu variable to setup the initial searhc address
DEFINE_PER_CPU(unsigned long *, kprobe_context_pointer);//this is used for steady state operations 

unsigned long* reference_offset = 0x0;


static int the_search(struct kretprobe_instance *ri, struct pt_regs *the_regs) { 

        unsigned long* temp = (unsigned long)&BRUTE_START;

        printk("%s: running the brute force search on CPU %d\n", MODNAME, smp_processor_id());

        while (temp > 0) {//brute force search of the current_kprobe per-CPU variable
                          //for enabling blocking execution of the kretprobe
                          //you can save this time setting up a per CPU-variable via 
                          //smp_call_function() upon module startup
                temp -= 1; 
#ifndef CONFIG_KRETPROBE_ON_RETHOOK
                if ((unsigned long) __this_cpu_read(*temp) == (unsigned long) &ri->rp->kp) {
#else
                if ((unsigned long) __this_cpu_read(*temp) == (unsigned long) &the_retprobe->kp) {
#endif
                        atomic_inc((atomic_t*)&successful_search_counter);//mention we have found the target 
                        printk("%s: found the target per-cpu variable (CPU %d) - offset is %p\n", MODNAME, smp_processor_id(),temp);
                        reference_offset = temp;//this assignment is done by multiple threads with no problem
                        break;
                }
                if(temp <= 0) return 1;
        }

        __this_cpu_write(kprobe_context_pointer, temp);

        return 0;
}


int init_module(void) {

	int ret;

	printk("%s: initializing\n",MODNAME);

        setup_probe.kp.symbol_name = setup_target_func;
        setup_probe.handler = NULL;
        setup_probe.entry_handler = (kretprobe_handler_t)the_search;
        setup_probe.maxactive = -1; //lets' go for the default number of active kretprobes manageable by the kernel

        ret = register_kretprobe(&setup_probe);
        if (ret < 0) {
                printk("%s: hook init failed for the init kprobe setup, returned %d\n", MODNAME, ret);
                return ret;
        }

        get_cpu();//this is redundant but you could use the returned CPU-id
        smp_call_function(run_on_cpu,NULL,1);


        if(successful_search_counter != (num_online_cpus() - 1)){
                printk("%s: read hook load failed - number of setup CPUs is %ld - number of remote online CPUs is %d\n", MODNAME, successful_search_counter, num_online_cpus() - 1);
                put_cpu();
                unregister_kretprobe(&setup_probe);
                return -1;

        }


        if (reference_offset == 0x0){
                printk("%s: inconsistemt value found for refeence offset\n", MODNAME);
                put_cpu();
                unregister_kretprobe(&setup_probe);
                return -1;
        }

        __this_cpu_write(kprobe_context_pointer, reference_offset);

        put_cpu();

	retprobe.kp.symbol_name = target_func;
        retprobe.handler = (kretprobe_handler_t)return_value;
        retprobe.entry_handler = (kretprobe_handler_t)sys_execve_wrapper;
        retprobe.maxactive = -1; //lets' go for the default number of active kretprobes manageable by the kernel

	the_retprobe = &retprobe;  

	ret = register_kretprobe(&retprobe);
        if (ret < 0) {
                printk("%s: kprobe registering failed, returned %d\n",MODNAME,ret);
                unregister_kretprobe(&setup_probe);
                return ret;
        }

	ret = 0;
	return ret;
}


#ifdef UNLOADABLE
void cleanup_module(void){
	unregister_kretprobe(&setup_probe);
	unregister_kretprobe(&retprobe);
        printk("%s: reference monitor unregistered\n",MODNAME);
}
#endif

