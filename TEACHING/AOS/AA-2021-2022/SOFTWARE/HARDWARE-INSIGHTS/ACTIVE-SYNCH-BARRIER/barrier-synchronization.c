/********************************************************************
 example of active synchronization barrier based on RMW instructionis
********************************************************************/



#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#ifndef THREADS
#define THREADS (16)
#endif


#ifndef CYCLES
#define CYCLES 10
#endif


long control_counter __attribute__((aligned(64))) = THREADS;
long era_counter __attribute__((aligned(64))) = THREADS;

__thread long int me;
__thread long int cycle;

int barrier(void){

	int ret;

	while(era_counter != THREADS && control_counter == THREADS);

	ret = __sync_bool_compare_and_swap(&control_counter,THREADS,0);
	if( ret ) era_counter = 0;

	__sync_fetch_and_add(&control_counter,1);
	while(control_counter != THREADS);
	__sync_fetch_and_add(&era_counter,1);

	return ret;
	
}



void* worker(void* id){
	int j;
	int ret;
	
	me = (long int)id;

	for (j= 0; j<CYCLES ; j++)	{
		ret = barrier();	
		cycle++;
		printf("cycle %d barrier done on thread %ld - initiator %d\n",j,(long int)id,ret);
	}

}

int main(int argc, char* argv){

	unsigned long i;
	pthread_t tid;


	for(i=0; i<THREADS; i++){

		pthread_create(&tid,NULL,worker,(void*)i);

	}

	pause();
}
