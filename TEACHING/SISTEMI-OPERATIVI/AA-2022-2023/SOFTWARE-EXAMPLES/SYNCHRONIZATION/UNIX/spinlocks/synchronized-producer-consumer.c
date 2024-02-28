#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


unsigned long global_lock   __attribute__ ((aligned (64))) = 0;


int lock(void * uadr){//this takes the address of the memory area where the lock variable resides
    unsigned long r =0 ;
    asm volatile(
        "xor %%rax,%%rax\n"
        "mov $1,%%rbx\n"
        "lock cmpxchg %%ebx,(%1)\n"
        "sete (%0)\n"
        : : "r"(&r),"r" (uadr)
        : "%rax","%rbx"
    );
    return (r) ? 1 : 0;
}


#define SIZE (100000)
#define END (10000000)

#define AUDIT if(0)

long v[SIZE] = {[0 ... (SIZE-1)] -1};

long counter = 0;

void * producer(void* dummy){

	long data = 0;
	long my_index = 0;

	printf("ready to produce\n");

retry:
	if(lock(&global_lock)){
	
		if(counter < SIZE){
			v[my_index] = data;
			my_index = (my_index+1)%SIZE;
			data++;
			counter++;
	
		}
		global_lock = 0;
	}

	goto retry;


}

void * consumer(void* dummy){

	long data = 0;
	long my_index = 0;
	long value;

	printf("ready to consume\n");

retry:
	if(lock(&global_lock)){
		if(counter > 0){
			value = v[my_index];

			AUDIT
			printf("consumer got value %d\n",value);

			if(value != data){
				printf("consumer: synch protocol broken at expected value: %d - real is %d!!\n",data,value);
				exit(EXIT_FAILURE);

			};

			if (value == END){
				printf("ending condition met - last read value is %d\n",value);
				exit(0);
			}

			my_index = (my_index+1)%SIZE;
			
			data++;
			counter--;

	
		}
		global_lock = 0;
	}
	goto retry;

}

int main(int argc, char** argv){

	pthread_t prod, cons;

	pthread_create(&cons,NULL,consumer,NULL);
//	sleep(1);
	pthread_create(&prod,NULL,producer,NULL);

	pause();

}

