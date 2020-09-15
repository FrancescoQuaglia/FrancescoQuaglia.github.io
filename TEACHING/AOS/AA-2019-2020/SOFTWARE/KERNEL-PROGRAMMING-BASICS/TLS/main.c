#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <asm/prctl.h>
#include <sys/prctl.h>

#include "tls.h"

typedef struct _entry{
	float a;
	long int x;
} entry;

PER_THREAD_MEMORY_START
	int x;
	int y;
	double z;
	entry k;
PER_THREAD_MEMORY_END

#define WORKERS 2

__thread long int v = 1;

int x = 1 ;

void *the_work(void * dummy){

	long int me = (long int)dummy;
	entry a;

	a.x = me;
	
	WRITE_THREAD_VARIABLE(k,a);	

	v = v*me;

	printf("worker %ld activated\n",(long int)(dummy));

	printf("(%d) variable x has value %d\n",me,x);
	printf("(%d) TLS variable x has value %d\n",me,READ_THREAD_VARIABLE(x));

	WRITE_THREAD_VARIABLE(x,x*me);
	printf("(%d) variable x has value %d\n",me,x);
	printf("(%d) TLS variable x has value %d\n",me,READ_THREAD_VARIABLE(x));

	printf("(%d) TLS variable v has value %ld\n",me,v);

	a = READ_THREAD_VARIABLE(k);
	printf("(%d) TLS variable has k.x field set to %ld\n",a.x);

	return NULL;
}


int main(int argc, char** argv){
	
	pthread_t tid;
	long int i;

	for(i=0;i<WORKERS;i++){
		pthread_create(&tid,NULL,the_work,(void*)i);
	}

	pause();

	return 0;

}
