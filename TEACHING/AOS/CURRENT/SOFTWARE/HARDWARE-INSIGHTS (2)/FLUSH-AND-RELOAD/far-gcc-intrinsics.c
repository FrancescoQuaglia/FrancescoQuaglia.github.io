/*********************************************************
  example usage of the flush-and-reload technique
  for inspecting the access latency to the memory hierarchy
**********************************************************/


#include <stdio.h>
#ifdef _MSC_VER
#include <intrin.h> /* for rdtscp and clflush */
#pragma optimize("gt",on)
#else
#include <x86intrin.h> /* for rdtscp and clflush */
#endif
#include <unistd.h>

char v[4096];

#define TRIALS (1<<15)


int main (int argc, char**argv){

	int i;
	char c;
	unsigned long cumulative_no_flush, cumulative_flush;
	unsigned long time1, time2;
	int junk = 0;

the_work:	

	sleep(1);

	cumulative_no_flush = 0;
	cumulative_flush = 0;

	for(i=0;i<4096;i++){
		    v[i] = 'f';
	}

	c = v[0];

	for(i=0;i<TRIALS;i++){
		time1 = __rdtscp( & junk);
		asm("lfence":::);
		c = v[0];
		asm("lfence":::);
		time2 = __rdtscp( & junk);
		cumulative_no_flush += time2 - time1;;
	}
	for(i=0;i<TRIALS;i++){
		_mm_clflush(v);
		time1 = __rdtscp( & junk);
		asm("lfence":::);
		c = v[0];
		asm("lfence":::);
		time2 = __rdtscp( & junk);
		cumulative_flush += time2 - time1;;
	}
	
	printf("no flush time is %u - flush time is %u\n",cumulative_no_flush/TRIALS, cumulative_flush/TRIALS);

	goto the_work;
	

}

