#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <asm/prctl.h>
#include <sys/prctl.h>
#include <unistd.h>



extern int arch_prctl(int code, unsigned long addr);

unsigned long other_TLS;
__thread char v[8]; 
__thread char v1[8]; 

void* the_thread(void* dummy){
        
	int i;
	unsigned long me = (unsigned long)dummy;
	unsigned long my_TLS_zone;

	arch_prctl(ARCH_GET_FS,(unsigned long)&my_TLS_zone);
	printf("(%d) my array V is at %p - my array V1 is at %p - my TLS zone is at %p\n",me,v,v1,my_TLS_zone);
	memset(v,(int)me,8);
retry:
	printf("(%d) my array is at %p\n",me,v);
	if(me == 0){
		other_TLS = my_TLS_zone;
	}
	else{
		
		memset(v,(long int)77,8);
		sleep(1);
		arch_prctl(ARCH_SET_FS,(unsigned long)other_TLS);
	}
	printf("(%d) my array content is %p\n",me,*v);
	sleep(2);
	goto retry;
}

int main (int argc, char**argv){

        pthread_t tid;

        pthread_create(&tid,NULL,the_thread,(void*)0);
        pthread_create(&tid,NULL,the_thread,(void*)1);
	pause();

}


