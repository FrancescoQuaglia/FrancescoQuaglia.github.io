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

#define target_func "sys_execve" //you should modify this depending on the kernel version

#define AUDIT if(1)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <francesco.quaglia@uniroma2.it>");
MODULE_DESCRIPTION("see the README file");

#define MODNAME "Reference monitor"


char *black_list[] = {"bash","tcsh","sh","sudo", NULL ,"su", NULL}; //the list of programs root cannot use to spawn others - being root is still admitted excluding 'su' from the list 

//#define STRONGER //this allows you to blacklist the above executables even for EUID-root users

#ifdef STRONGER
#define	corrector (current->cred->euid.val == 0)
#else
#define	corrector (0)
#endif

#define MAX_LEN 256

static int sys_execve_wrapper(struct kprobe *ri, struct pt_regs *regs){

	char command[MAX_LEN];
	int i=-1;
	int ret = 0;

	do{
	    ++i;
	    ret = copy_from_user(&(command[i]),&(((char*)(regs->di))[i]),1);
	}while(i<(MAX_LEN-1) && (command[i] != '\n')&&(ret==0));
	command[i]='\0';

	printk("%s: --------------------------------------------\n",MODNAME);
	printk("%s: this is the wrapper for execve - trying to run: %s\n",MODNAME,command);
	printk("%s: request on behalf of user %d - euid %d (current command is: %s)\n",MODNAME,current->cred->uid.val,current->cred->euid.val,command);

	if(current->cred->uid.val == 0 || corrector){
		printk("%s: need black list search\n",MODNAME);
		for(i=0; black_list[i] != NULL; i++){
			if(strcmp(black_list[i],current->comm) == 0 ){
				AUDIT
				printk("%s: current couple <program,UID> is black listed in the UID domain - execve rejected\n",MODNAME);//the domin can include EUID specification depedning in the value of 'corrector'
				goto reject;
			}
		}
		AUDIT
		printk("%s: current couple <program,UID> can run %s according to domain specification - finally executing the requested execve \n",MODNAME,command);
		
	}	

	return 0;

reject:
	regs->di = (unsigned long)NULL;
	return 0;

}

static struct kprobe kp = {
        .symbol_name =  target_func,
        .pre_handler = sys_execve_wrapper,
};


int init_module(void) {

	int ret;

	printk("%s: initializing\n",MODNAME);

	ret = register_kprobe(&kp);
        if (ret < 0) {
                printk("%s: kprobe registering failed, returned %d\n",MODNAME,ret);
                return ret;
        }

	ret = 0;

	return ret;
}



