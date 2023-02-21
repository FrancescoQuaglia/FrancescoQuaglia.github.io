/* 
  try this program with different inputs and by relying on the chrt shell command
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <sys/time.h>
#include <sys/resource.h>

unsigned long spawns;
unsigned long busy_loop;

#define SCALING 1000

#ifdef AUTO_AFFINITY
#include <sched.h>
ulong CPU_set = 0x1; 
cpu_set_t *mask = &CPU_set;
#endif

void * child_thread(void*p){
	int i;
	unsigned long id;
	int round=0;

	id = (unsigned long)p;

	while(1){
		for (i= 0; i< SCALING*busy_loop; i++);
	}
}

int main(int argc, char** argv){

   int i;
   int status;
   void **thread_status;
   pthread_t tid;
   int prio;
   int ret;

   if(argc<3){
	printf("usage: command spawns busy-loop-length\n");
	exit(EXIT_FAILURE);
   }

#ifdef AUTO_AFFINITY
	ret = sched_setaffinity(getpid(),sizeof(ulong),mask);
	printf("affinity set returned with code %d\n",ret);
#endif

   spawns = strtol(argv[1],NULL,10);
   busy_loop = strtol(argv[2],NULL,10);
	
   printf("running with spawns set to %lu and busy-loop-length set to %lu\n",spawns,busy_loop);

   for (i=0;i<spawns;i++){
	  //pthread_create(&tid,NULL,child_thread,(void*)((unsigned)i));
	  pthread_create(&tid,NULL,child_thread,NULL);
   }
 

   while(1){
	printf("please give me the priority level you would prefer\n");
	scanf("%d",&prio);
	//ret = setpriority(PRIO_PROCESS,0,prio);
	ret = setpriority(PRIO_PGRP,0,prio);
	printf("priority set returns %d\n",ret);
	ret = getpriority(PRIO_PROCESS,0);
	printf("new priority is %d\n",ret);
   }

}
