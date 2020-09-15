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
* @file usctm.c 
* @brief This is the main source for the Linux Kernel Module which implements
* 	 the runtime discovery of the syscall table position and of free entries (those 
* 	 pointing to sys_ni_syscall) - the only assumption is that we have access to the
* 	 compiled version of the kernel (even under randomization)
*
* @author Francesco Quaglia, Romolo Marotta
*
* @date November 8, 2019
*/

#define EXPORT_SYMTAB
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
#include "./include/vtpmo.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia <quaglia@dis.uniroma1.it>, Romolo Marotta <marotta@diag.uniroma1.it>");
MODULE_DESCRIPTION("USCTM");


#define MODNAME "USCTM"


extern int sys_vtpmo(unsigned long vaddr);


#define ADDRESS_MASK 0xfffffffffffff000//to migrate

#define START 			0xffffffff00000000ULL		// use this as starting address --> this is a biased search since does not start from 0xffff000000000000
#define MASK_12 		0x0000000000000fffULL		// mask to get least 12 bits
#define ALIGN_12_INC		(MASK_12+0x1ULL)		// value to iterate over entries of size equal to 2**12
#define MASK_21 		0x00000000001fffffULL		// mask to get least 21 bits
#define ALIGN_21_INC		(MASK_21+0x1ULL)		// value to iterate over entries of size equal to 2**21
#define MAX_ADDR		0xfffffffffff00000ULL
#define FIRST_NI_SYSCALL	134
#define SECOND_NI_SYSCALL	174

#define ENTRIES_TO_EXPLORE 256


// offset between text and rodata sections in the kernel image (module parameter)
#define OFFSET			-1 //0xE00000
int myoffset = OFFSET;
module_param(myoffset, int, 0);

unsigned long *hacked_ni_syscall=NULL;
unsigned long **hacked_syscall_tbl=NULL;

unsigned long sys_call_table_address = 0x0;
module_param(sys_call_table_address, ulong, 0660);

unsigned long sys_ni_syscall_address = 0x0;
module_param(sys_ni_syscall_address, ulong, 0660);


/* This routine checks if the page contains the begin of the syscall_table.
   It has the following assumption:
	6) syscall_table is aligned to sizeof(void*) 
	7) entries 134 and 174 of the syscall table are occupied by sys_ni_syscall
	8) syscalls are aligned to 8B
*/


#define PAGE_SIZE 4096

int validate_page(unsigned long *addr){
	int i = 0;
	unsigned long page 	= (unsigned long) addr;
	unsigned long new_page 	= (unsigned long) addr;
	for(; i < PAGE_SIZE; i+=sizeof(void*)){		// uses assumption 6)
		new_page = page+i+SECOND_NI_SYSCALL*sizeof(void*);
			
		// If the table occupies 2 pages check if the second one is materialized in a frame
		if( 
			( (page+PAGE_SIZE) == (new_page & ADDRESS_MASK) )
			&& sys_vtpmo(new_page) == NO_MAP
		) 
			break;
		// go for patter matching
		addr = (unsigned long*) (page+i);
		if(
			   ( (addr[FIRST_NI_SYSCALL] & 0x3  ) == 0 )			// uses assumption 8)	
			&& ( (addr[FIRST_NI_SYSCALL] & START) == START ) 		// uses assumption 4)
			&&    addr[FIRST_NI_SYSCALL] == addr[SECOND_NI_SYSCALL]		// uses assumption 7)
		){
			// it matches
			hacked_ni_syscall = (void*)(addr[FIRST_NI_SYSCALL]);				// save ni_syscall
			sys_ni_syscall_address = (unsigned long)hacked_ni_syscall;
			hacked_syscall_tbl = (void*)(addr);				// save syscall_table address
			sys_call_table_address = (unsigned long) hacked_syscall_tbl;
			return 1;
		}
	}
	return 0;
}

/* This routines looks for the syscall table.
 * It applies a biased search. 
 * It assumes that:
 *	1) segments of the kernel are aligned to 2**21 B (this comes from compiled kernel inspection)
 *	2) syscall_table is in rodata section of the linux image
 *	3) when loaded in memory there is no randomization within a specific segment (as it is normally)
 *	4) the segment will be placed in an address equal to or greater than 0xffffffff00000000 
 *	5) the syscall_table can be located by starting within the first page of rodata section
 */
void syscall_table_finder(void){
	unsigned long k; // current page
	unsigned long candidate; // current page

	// starting from the initial address look for a couple of materialized pages whose distance is equal to OFFSET
	// check only pages aligned to 2**21
	for(k=START; k < MAX_ADDR; k+=ALIGN_21_INC){	// use assumption 1) & 4)
		candidate = k+myoffset;
		if(
			(sys_vtpmo(k) != NO_MAP) 		// candidate for text section
			&& (sys_vtpmo(candidate) != NO_MAP)	// candidate for rodata section
		){
			// both are materialized 
			// check if candidate maintains the syscall_table
			if(validate_page( (unsigned long *)(candidate)) ){
				printk("%s: syscall table found at %px\n",MODNAME,(void*)(hacked_syscall_tbl));
				printk("%s: sys_ni_syscall found at %px\n",MODNAME,(void*)(hacked_ni_syscall));
				break;
			}
		}
	}
	
}


#define MAX_FREE 10
int free_entries[MAX_FREE];
module_param_array(free_entries,int,NULL,0660);//default array size already known - here we expose what entries are free

int init_module(void) {
	
	int i,j;
	
	if(myoffset < 0){
		printk("%s: invalid offset passed in input\n",MODNAME);
		return -1;
	}
		
        printk("%s: initializing\n",MODNAME);
	
	syscall_table_finder();

	if(!hacked_syscall_tbl){
		printk("%s: failed to find the sys_call_table\n",MODNAME);
		return -1;
	}

	j=0;
	for(i=0;i<ENTRIES_TO_EXPLORE;i++)
		if(hacked_syscall_tbl[i] == hacked_ni_syscall){
			printk("%s: found sys_ni_syscall entry at syscall_table[%d]\n",MODNAME,i);	
			free_entries[j++] = i;
			if(j>=MAX_FREE) break;
		}

        printk("%s: module correctly mounted\n",MODNAME);

        return 0;

}

void cleanup_module(void) {
                
        printk("%s: shutting down\n",MODNAME);
        
}
