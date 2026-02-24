/* try this program with different setting for AUTO_AFFINITY (on/off) and 
   by using the taskset shell command 
*/


#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

unsigned long spawns;
unsigned long busy_loop;

#define SCALING 1000

#ifdef AUTO_AFFINITY
#include <sched.h>
ulong CPU_set = 0x1; 
cpu_set_t *mask = (cpu_set_t*)&CPU_set;
#endif

void * child_thread(void*p){
	int i;
	unsigned int id;
	int round=0;

	id = (unsigned long)p;

	while(1){
		for (i= 0; i< SCALING*busy_loop; i++);
		printf("thread %u is alive - round %d\n",id,round++);		
	}
}

int main(int argc, char** argv){

   int i;
   int status;
   void **thread_status;
   pthread_t tid;

   if(argc<3){
	printf("usage: command spawns busy-loop-length\n");
	exit(EXIT_FAILURE);
   }

#ifdef AUTO_AFFINITY
	int ret;
	ret = sched_setaffinity(getpid(),sizeof(ulong),mask);
	printf("affinity set returned with code %d\n",ret);
#endif

   spawns = strtol(argv[1],NULL,10);
   busy_loop = strtol(argv[2],NULL,10);
	
   printf("running with spawns set to %lu and busy-loop-lenght set to %lu\n",spawns,busy_loop);

   for (i=0;i<spawns;i++){
	  pthread_create(&tid,NULL,child_thread,(void*)((unsigned long)i));
   }
 
  pause();

}
