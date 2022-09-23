/******************************************************
this file ia an implemntatin of the Lamport's bakery algorithm
for manipulating atomically a counter value

depending on the actual unerlying hardware the algorithm can be broken
******************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define SPAWNS 2 //number of threads to spawn
#define TOCKENS 1000000 //starting value of the share counter
#define TIME (1) //time span of the test in seconds

#define AUDIT if(0)


volatile int numbers[SPAWNS] __attribute__((aligned (64)))= { [0 ... (SPAWNS-1)] = 0 };
volatile int choosing[SPAWNS] __attribute__((aligned (64)))= { [0 ... (SPAWNS-1)] = 0 };


long tokens_to_distribute = TOCKENS;

int end = 0;

void* child_thread(void*p){

	int me = (long)p;
	long max, i, got, num;

	AUDIT
	printf("I'm child %d - running Lamport's Bakery\n",me);
	
	while(1){
		max = 0;
		choosing[me] = 1;
		for(i=0; i<SPAWNS; i++){
			if(max < numbers[i]) max = numbers[i];	
		}
		num = numbers[me] = max + 1;
		choosing[me] = 0;

		for(i=0; i<SPAWNS; i++){
			while(choosing[i]) {};
			while((numbers[i]!=0) && ((numbers[i]<numbers[me])||((numbers[i]==numbers[me])&&(i<me)))) {}; 
		}
		got = tokens_to_distribute--;
		numbers[me] = 0;	

		AUDIT
		printf("I'm child %d - number %d - done critical region with got set to %d\n",me,num,got);

		printf("%ld\n",got);//this output is requested for the overall test
		if(end) pthread_exit(NULL);
	}

}

int main(int argc, char** argv){

   long i;
   int status;
   void **thread_status;
   pthread_t tid;
  
   for (i=0;i<SPAWNS;i++){
	  pthread_create(&tid,NULL,child_thread,(void*)i);
	}

   sleep(TIME);
   end = 1;
   sleep(TIME);
}

