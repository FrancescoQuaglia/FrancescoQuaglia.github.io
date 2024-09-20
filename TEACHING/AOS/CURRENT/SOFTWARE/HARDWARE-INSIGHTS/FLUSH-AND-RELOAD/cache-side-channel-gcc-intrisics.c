#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#ifdef _MSC_VER
#include <intrin.h> /* for rdtscp and clflush */
#pragma optimize("gt",on)
#else
#include <x86intrin.h> /* for rdtscp and clflush */
#endif



char v[8096] __attribute__((aligned(64)));



void* cache_filler(void* dummy){
	
	int i;
	volatile char x;
	volatile char y;
	volatile char z;

	

	for(i=0;i<4096;i++){
		    v[i] = 'f'; 
	}

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
	char c;
	unsigned long time1, time2, time;
        int junk = 0;
	

	pthread_create(&tid,NULL,cache_filler,NULL);

new_trial:
	
	printf("give me the displacement from V[] you would like to inspect: ");
	scanf("%d",&displacement);

	_mm_clflush(v+displacement);
	sleep(1);//let the victim have time to refill the cache
	time1 = __rdtscp( & junk);
        asm("lfence":::);
        c = *(v+displacement);
        asm("lfence":::);
        time2 = __rdtscp( & junk);
        time = time2 - time1;;

	if (time < 100){
		printf("byte displacement %d has been accessed - cache access delay is %d\n",displacement,time);
	}
	else{ 
		printf("byte displacement %d has not been accessed - cache access delay is %d\n",displacement,time);
	}
	
	goto new_trial;

}

