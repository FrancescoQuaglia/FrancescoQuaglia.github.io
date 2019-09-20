#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define PAGE_SIZE (4096)
#define NR_CONTEXTS 2
#define STACK_SIZE (PAGE_SIZE<<2)

void * stacks[NR_CONTEXTS];

long scheduler_setup(void){
	return syscall(134);
}

long setup_thread(unsigned long start_routine, unsigned long the_stack, int type){//type 0 regular - type 1 idle
	return syscall(174,start_routine,the_stack,type);
}

#define schedule() asm volatile("push %%rax; mov $0,%%rax ; int $0xf5; pop %%rax":::) 

#define terminate() asm volatile("push %%rax; mov $2,%%rax ; int $0xf5; pop %%rax":::) 

#define run_scheduler() asm volatile("push %%rax; mov $1,%%rax ; int $0xf5; pop %%rax":::) 

void idle(void){
redo:

	printf("idle thread running\n");
	schedule();
	goto redo;
	return;
}

void *the_flow(void*dummy){
	run_scheduler();
	printf("should never get here\n");
	return NULL;
}



void the_thread(void){

	int round = 0;;

again:
	printf("I'm at round %d\n",round++);
	while(1);
	sleep(1);

	schedule();

	if(round == 5) terminate();

	goto again;
}


_start(){
	int i;
	int ret;
	pthread_t tid;
	void* stack;
	int num_workers;

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
		ret = setup_thread((unsigned long)idle, (unsigned long)stacks[i],1);
		printf("thread %d, setup returned %d\n",i,ret);
	}
	
	for (i=0;i<NR_CONTEXTS;i++){
		pthread_create(&tid,NULL,the_flow,NULL);
	}

back:

	printf("how many workers would you like to run?\n");

	scanf("%d",&num_workers);

	for(i=0;i<num_workers;i++){

		stack = malloc(STACK_SIZE);
		if(stack == NULL) {
			printf("could not setup new worker thread stack\n");
			exit(-1);
		}
		setup_thread((unsigned long)the_thread,(unsigned long)(stack+STACK_SIZE),0);
	}

	goto back;

	return 0;

}

