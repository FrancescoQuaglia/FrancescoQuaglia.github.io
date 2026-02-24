#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int* aux;//pointer used to update the stack of a different thread by the interfering_child_thread function

void* child_thread(void*p){

	int c = 1;
	aux = &c;

	while(1){
		printf("variable c has value: %d\n",c);
		sleep(2);
	}
}


void* interfering_child_thread(void*p){

	int c;

	while(1){
		scanf("%d",&c);
		*aux = c;
	}

}
int main(int argc, char** argv){

   	pthread_t tid;

	if( pthread_create(&tid,NULL,child_thread,NULL) != 0 ){
		printf("pthreadcreate error\n");
        	fflush(stdout);
        	exit(EXIT_FAILURE);
   	}


	if( pthread_create(&tid,NULL,interfering_child_thread,NULL) != 0 ){
		printf("pthreadcreate error\n");
       		fflush(stdout);
        	exit(EXIT_FAILURE);
   	}

	pause();
}
