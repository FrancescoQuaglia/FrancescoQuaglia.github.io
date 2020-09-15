#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <asm/prctl.h>
#include <sys/prctl.h>


#define DEBUG if(0)

extern int TLS_SIZE;
extern unsigned long tls_position(void);

extern int __real_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg);
extern int arch_prctl(int code, unsigned long addr);


typedef struct _target {
	void *(*the_start_routine)(void*);
	void * the_arg;
} target;

void* tls_setup(void*);

int __wrap_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg){

	target *trg;
	int ret;

	trg = (target*)malloc(sizeof(target));

	trg->the_start_routine = start_routine; 
	trg->the_arg = arg;

	DEBUG
	printf("I'm the wrapper - target function is at address %p\n",trg->the_start_routine);
	
 	ret = __real_pthread_create(thread,attr,tls_setup,(void*)trg);	

	return ret;

}


void * tls_setup(void * args){
	

	unsigned long addr;
	void *(*the_start_routine)(void*);
	void * the_arg;

	addr = (unsigned long)mmap(NULL, sizeof(TLS_SIZE),PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,0,0);

	*(unsigned long*)addr = addr;

	arch_prctl(ARCH_SET_GS,addr);

	DEBUG{
		printf("auditing the setup: TLS is at %p - control pointer points to %p - get pointer returned %p\n",addr,*(unsigned long*)addr,tls_position());
		fflush(stdout);
	}	
	the_start_routine = ((target*)args)->the_start_routine;

	the_arg = ((target*)args)->the_arg;

	DEBUG
	printf("I'm about to call at address %p\n",the_start_routine);

	fflush(stdout);

	return the_start_routine(the_arg);

}
