#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


char v[8096] __attribute__((aligned(64)));

unsigned long probe(char* adrs){

	volatile unsigned long cycles;


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
		"clflush 0(%1)	\n"
		: "=a" (cycles)
		: "c" (adrs) 
		: "%esi" , "%edx" );

	return cycles;
}



void* cache_filler(void* dummy){
	
	int i;
	volatile char x;
	volatile char y;
	volatile char z;

	


	while(1){
		
		x = v[0];//reading the 0-th cache line hosting the array (64 bytes in L1) 
	//	y = v[64];//reading the 1-st cache line hosting the array (64 bytes in L1) 
	//	y = v[128];//reading the 2-nd cache line hosting the array (64 bytes in L1) 
			//beware cache prefetch anyhow

	}	

	return NULL;
}

int main (int argc, char**argv){

	int i;
	unsigned displacement;
	int ret;
	pthread_t tid;

	for(i=0;i<4096;i++){
		    v[i] = 'f'; 
	}

	pthread_create(&tid,NULL,cache_filler,NULL);


new_trial:

	
	printf("give me the displacement from V[] you would like to inspect: ");
	scanf("%d",&displacement);

	ret = probe(v+displacement);
	sleep(2);//let the victim have time to refill the cache
	ret = probe(v+displacement);
	if (ret < 100){//use the right threshold value on your machine
		printf("byte displacement %d has been accessed - cache access delay is %d\n",displacement,ret);
	}
	else{ 
		printf("byte displacement %d has not been accessed - cache access delay is %d\n",displacement,ret);
	}
	
	goto new_trial;

}

