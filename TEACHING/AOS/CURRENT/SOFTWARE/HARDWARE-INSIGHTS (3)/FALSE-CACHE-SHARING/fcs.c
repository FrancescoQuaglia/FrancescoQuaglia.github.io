/***************************************************************************
 example of false cache sharing performance problems 
 please try with different values of TARGET
***************************************************************************/ 


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SPAWNS 2 
#define CYCLES 500000000

#ifndef TARGET
#define TARGET 1
#endif

int v[128] __attribute__((aligned(64)));
int sum;

void* child_thread(void*p){

	int me = (long)p;
	int i;

	if (me == 0 ){ 
		printf("I'm child %d - the cache line writer\n",me);
		for(i=0; i<CYCLES; i++){
			v[0]+=i;	
			asm("mfence":::);
		}
	}
	else{

		printf("I'm child %d - the cache line reader\n",me);
		for(i=0; i<CYCLES; i++){
			sum += v[TARGET];	
			asm("mfence":::);
		}
	}
	
	
}

int main(int argc, char** argv){

   long i;
   int status;
   void *thread_status;
   pthread_t tid;
  
   for (i=0;i<SPAWNS;i++){
	  pthread_create(&tid,NULL,child_thread,(void*)i);
	}

   pthread_join(tid,&thread_status); //wait for the last spawned thread
}

