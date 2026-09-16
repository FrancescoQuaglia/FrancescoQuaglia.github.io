/*********************************************************
  example usage of the flush-and-reload technique
  for inspecting the access latency to the memory hierarchy
**********************************************************/


#include <stdio.h>

char v[4096];

#define TRIALS (1<<20)

unsigned long probe(char* adrs){

	volatile unsigned long cycles;

	asm volatile(
		"mfence	\n"
		"lfence \n"
		"rdtsc	\n"
		"lfence	\n"
		"movl %%eax,%%esi	\n"
		"movl (%1), %%eax	\n"
		"lfence	\n"
		"rdtsc 	\n"
		"subl %%esi, %%eax	\n"	
		"clflush 0(%1)	\n"
		: "=a" (cycles)
		: "c" (adrs) 
		: "%esi" , "%edx" );

	return cycles;
}



int probe_no_flush(char* adrs){

	volatile unsigned long time;

	asm(
		"mfence	\n"
		"lfence \n"
		"rdtsc	\n"
		"lfence	\n"
		"movl %%eax,%%esi	\n"
		"movl (%1), %%eax	\n"
		"lfence	\n"
		"rdtsc 	\n"
		"subl %%esi, %%eax	\n"	
		: "=a" (time)
		: "c" (adrs)
		: "%esi" , "%edx" );

	return time;
}


int main (int argc, char**argv){

	int i;
	char c;
	unsigned long cumulative_no_flush, cumulative_flush;

	cumulative_no_flush = 0;
	cumulative_flush = 0;

	for(i=0;i<4096;i++){
		    v[i] = 'f';
	}

	c = v[0];

	for(i=0;i<TRIALS;i++){
		cumulative_no_flush += probe_no_flush(v);
	}
	for(i=0;i<TRIALS;i++){
		cumulative_flush += probe(v);
	}
	
	printf("no flush time is %u - flush time is %u\n",cumulative_no_flush/TRIALS, cumulative_flush/TRIALS);
	

}

