#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

char dummy[8];
__thread char v[8]; 
char *p = dummy;

void* the_thread(void* dummy){
        
	int i;
	unsigned long me = (unsigned long)dummy;

	printf("(%d) my array is at %p\n",me,v);
	memset(v,(long int)me,8);
retry:
	if(me == 0){
		p = v;
	}
	else{
		memset(p,(long int)me,8);
	}
	printf("(%d) my array content is %p\n",me,*v);
	sleep(1);
	goto retry;
}

int main (int argc, char**argv){

        pthread_t tid;


        pthread_create(&tid,NULL,the_thread,(void*)0);
        pthread_create(&tid,NULL,the_thread,(void*)1);
	pause();

}


