#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#ifndef NUM_THREADS
#define NUM_THREADS 10
#endif

#define MAX_STRING_LEN     (4096 << 1)

#ifndef NUM_TARGET_STRINGS
#define NUM_TARGET_STRINGS 1
#endif

char* the_strings[NUM_TARGET_STRINGS] ; 

__thread char buffer[MAX_STRING_LEN];
__thread int occurrences = 0;

void * SearchThread(){

  int status;
  int i;

  while(scanf("%s",buffer) == 1){
	for(i=0;i<NUM_TARGET_STRINGS;i++){
		if(strstr(buffer,the_strings[i]) != NULL){
			occurrences++;
		}
	}
  }	
  printf("found %d occurrences of searched strings\n",occurrences);
  status = 0;
  pthread_exit((void *)&status);
} 

int main (int a, char **b ) {
  int i;
  pthread_t tid[NUM_THREADS];
  long thread_count = 0;

  if (a != 2 ){
	printf("usage: prog_name num_thread\n");
	return 1;
  }

  thread_count = strtol(b[1],NULL,10);
  printf("running with %d threads\n",thread_count);
  fflush(stdout);
 
  for(i=0; i< NUM_TARGET_STRINGS; i++){
	buffer[0] = '\0';
	scanf("%s",buffer);
	the_strings[i] = strdup(buffer);
        if (the_strings[i] == NULL) exit(EXIT_FAILURE);
  }

  for(i=0; i<thread_count; i++){
	pthread_create(&tid[i],NULL,SearchThread,NULL);
  }
  for(i=0; i<thread_count; i++){
	pthread_join(tid[i],NULL);
  }
  exit(0);
}

