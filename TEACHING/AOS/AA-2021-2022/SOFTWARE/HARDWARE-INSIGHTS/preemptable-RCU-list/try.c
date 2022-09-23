#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <unistd.h>
#include "list.h"

#ifndef THREADS
#define THREADS (5)
#endif


#define OPERATIONS (8<<11)

list the_list;


void * worker(void* id){
	int j;
	int ret;

	if ((long)id == 0){
		for (j= 0; j<OPERATIONS ; j++)	{
			ret = list_insert(&the_list,j);
			AUDIT
			printf("thread %d - inserting key %d - result %d\n",(long)id,j,ret);
		}	
		AUDIT
		printf("thread %d - insertions done\n",(long)id);
		return NULL;
	}
	if ((long)id == 1){
		AUDIT
		sleep(1);
		for (j= 0; j< 2*OPERATIONS ; j++)	{
			ret = list_remove(&the_list,j);
			//ret = list_remove(&the_list,-1);//this leads to a list scan with no success
			AUDIT
			printf("thread %d - removing key %d - result %d\n",(long)id,j,ret);
		}	
		AUDIT
		printf("thread %d - removals done\n",(long)id);
		return NULL;
	}

	AUDIT
	sleep(1);
	for (j = 0; j < 2 * OPERATIONS ; j++)	{
		ret = list_search(&the_list,j);
		AUDIT
		printf("thread %d - scanning the list for key %d - result %d\n",(long)id,j,ret);
	}

	return NULL;
}

int main(int argc, char* argv){

	unsigned long i;
	pthread_t tid[THREADS];

	list_init(&the_list);


	for(i=0; i<THREADS; i++){

		pthread_create(&(tid[i]),NULL,worker,(void*)i);
		if(i == 0) pthread_join(tid[i],NULL);

	}

	for(i=0; i<THREADS;i++){
		pthread_join(tid[i],NULL);
	}
}
