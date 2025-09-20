/* try this program with different setting for AUTO_AFFINITY (on/off) and 
   by using the taskset shell command 
   please compile with -lnuma
*/


#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <numaif.h>
#include <sys/types.h>
#include <unistd.h>


#define NUM_PAGES 5000 

#define AUTO_AFFINITY

#ifdef AUTO_AFFINITY
#include <sched.h>
ulong CPU_set = 0x2; 
cpu_set_t *mask = (cpu_set_t*)&CPU_set;
#endif

ulong node_set = 0x1; //NUMA node to which memory will be bound

char buff[4096*NUM_PAGES];

void flush(char* adrs){

        asm(
                "mfence \n"
                "clflush 0(%0)  \n"
                : 
                : "a" (adrs)
                :  );

        return ;
}


int main(int argc, char** argv){

   int i,j;
   char c;
   pthread_t tid;

   set_mempolicy(MPOL_BIND,&node_set,sizeof(ulong));
   for (i=0;i<NUM_PAGES;i++){
	buff[i*4096] = 'q';
   }


#ifdef AUTO_AFFINITY
	int ret;
	ret = sched_setaffinity(getpid(),sizeof(ulong),mask);
	printf("affinity set returned with code %d\n",ret);
#endif

	
   for (i=0;i<NUM_PAGES;i++){
	for(j=0;j<4096;j++){
		buff[i*4096+j] = 'f';
		flush(&buff[i*4096+j]);	
	}
   }
}

