#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define PAGE_SIZE (4096)
#define NR_CONTEXTS 4
#define STACK_SIZE (PAGE_SIZE<<2)

void * stacks[NR_CONTEXTS];

void the_thread(void){

again:
	printf("I'm the thread\n");
	sleep(1);

	goto again;
}


_start(){
	int i;

	for (i=0;i<NR_CONTEXTS;i++){
		stacks[i] = malloc(STACK_SIZE);
		if (stacks[i] == NULL) exit(-1);
	}

	printf("stacks setup done\n");

	for (i=0;i<NR_CONTEXTS;i++){
		//asm volatile("push %%rsp ; mov %0, %%rsp; mov %%rbx, %%r15; int $0xf5; pop %%rsp"::"a" (stacks[i]) , "b" (the_thread):);
	asm volatile("mov %%rax,%%r15 ; mov %%rbx, %%r14; int $0xf5"::"a" (the_thread) , "b" (stacks[i]):);
	//	asm volatile("push %%rsp ; mov %0, %%rsp; mov %%rbx, %%r15; int $0xf5; pop %%rsp"::"a" (stacks[i]) , "b" (the_thread):);
	}
	
	sleep(2);
	write(1,"awake\n",6);
	asm volatile("mov %%rax,%%r15 ; int $0xf5"::"a" (_start):);
	exit(0);
}

