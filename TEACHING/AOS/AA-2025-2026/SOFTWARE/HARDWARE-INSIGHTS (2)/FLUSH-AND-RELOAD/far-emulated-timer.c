/*********************************************************
  example usage of the flush-and-reload technique
  for inspecting the access latency to the memory hierarchy
**********************************************************/


#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

char v[4096];

#define TRIALS (1<<3)

unsigned long timer;

void * time_keeper(void * dummy){

	while(1){
		timer = (timer+1);
	}	
}

unsigned long probe(char* adrs, unsigned long * timer){

	volatile unsigned long cycles;

	asm(
		"mfence	\n"
		"clflush 0(%1)	\n" 		//flush the target cache line
		"mfence	\n"
		"mov (%2),%%rdx	\n" 		//read timer initial value
		"lfence	\n"
		"mov (%1), %%esi	\n" 	//read again the cache line
		"mfence \n"
		"mov (%2),%%rax	\n"		//read again the timer value
		"mfence	\n"
		"sub %%rdx, %%rax	\n"	//compute elapsed time	
		: "=a" (cycles)
		: "c" (adrs) , "b" (timer));

	return cycles;
}



int probe_no_flush(char* adrs , unsigned long * timer){

	volatile unsigned long cycles;

	asm(
		"mfence	\n"
		"mov (%2),%%rdx	\n" 		//read timer initial value
		"lfence	\n"
		"mov (%1), %%esi	\n" 	//read again the cache line
		"mfence \n"
		"mov (%2),%%rax	\n"		//read again the timer value
		"sub %%rdx, %%rax	\n"	//compute elapsed time	
		: "=a" (cycles)
		: "c" (adrs) , "b" (timer));

	return cycles;
}


int main (int argc, char**argv){

	int i;
	char c;
	unsigned long cumulative_no_flush, cumulative_flush;
	pthread_t tid;


	pthread_create(&tid,NULL,time_keeper,NULL);


the_work:	

	sleep(1);

	cumulative_no_flush = 0;
	cumulative_flush = 0;

	for(i=0;i<4096;i++){
		    v[i] = 'f';
	}

	c = v[0];

	for(i=0;i<TRIALS;i++){
		cumulative_no_flush += probe_no_flush(v,&timer);
	}
	for(i=0;i<TRIALS;i++){
		cumulative_flush += probe(v,&timer);
	}
	
	printf("no flush time is %u - flush time is %u\n",cumulative_no_flush/TRIALS, cumulative_flush/TRIALS);

	goto the_work;
	

}

