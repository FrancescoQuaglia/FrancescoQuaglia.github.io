#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define PAGE_SIZE (4096)
#define NR_CONTEXTS 4
#define STACK_SIZE (PAGE_SIZE<<2)

void * stacks[NR_CONTEXTS];

long scheduler_setup(void){
	return syscall(134);
}

long setup_thread(unsigned long start_routine, unsigned long the_stack){
	return syscall(174,start_routine,the_stack);
}

#define schedule() asm volatile("push %%rax; mov $0,%%rax ; int $0xf5; pop %%rax":::) 

void the_thread(void){

	int round = 0;;
again:
	printf("I'm at round %d\n",round++);

	sleep(1);

	schedule();

	goto again;
}


int _start(){
	int i;
	int ret;

	for (i=0;i<NR_CONTEXTS;i++){
		stacks[i] = malloc(STACK_SIZE);
		if (stacks[i] == NULL) exit(-1);
		memset(stacks[i],0,STACK_SIZE);
		stacks[i] = (stacks[i] + STACK_SIZE);
	}

	printf("threads' stacks setup done\n");
	ret = scheduler_setup();
	printf("thread scheduler setup returned %d\n",ret);
	

	for (i=0;i<NR_CONTEXTS;i++){
		ret = setup_thread((unsigned long)the_thread, (unsigned long)stacks[i]);
		printf("thread %d, setup returned %d\n",i,ret);
	}
	
back:
	schedule();
	pause();
	goto back;
	exit(0);
	return 0;
}

