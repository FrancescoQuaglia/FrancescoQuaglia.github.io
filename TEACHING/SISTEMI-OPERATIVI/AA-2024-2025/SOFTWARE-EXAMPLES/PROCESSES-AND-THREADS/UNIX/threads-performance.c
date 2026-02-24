#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define SPAWNS 50000

void* child_thread(void*p){
	return NULL;
}

int main(int argc, char** argv){

   int i;
   int status;
   void *thread_status;
   pthread_t tid;

   if (argv[1]==NULL){
	printf("missing arg[1]\n");
	exit(EXIT_FAILURE);
   }
  
   if (strcmp("processes", argv[1]) ==0) {
	for (i=0;i<SPAWNS;i++){
          if(fork()) wait(&status);
	  else exit(0);		
	}
   }

   if (strcmp("threads", argv[1]) ==0) {
	for (i=0;i<SPAWNS;i++){
	  pthread_create(&tid,NULL,child_thread,NULL);
	  pthread_join(tid,&thread_status);
	}

   }
}
