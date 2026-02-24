#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>



#define SIZE (1000)
#define END (10000000)

#define AUDIT if(0)

long *v;

sem_t * sem_free_slot, * sem_available_item;

void * producer(void){


	long data = 0;
	long my_index = 0;

	printf("ready to produce\n");

retry:

	sem_wait(&(sem_free_slot[my_index]));
	
	v[my_index] = data;

	sem_post(&(sem_available_item[my_index]));

	my_index = (my_index+1)%SIZE;
	data++;

	goto retry;


}

void * consumer(void){

	long data = 0;
	long my_index = 0;
	long value;

	printf("ready to consume\n");

retry:
	sem_wait(&(sem_available_item[my_index]));
	value = v[my_index];

	AUDIT
	printf("consumer got value %ld\n",value);

	if(value != data){
		printf("consumer: synch protocol broken at expected value: %ld - real is %ld!!\n",data+1,value);
		exit(EXIT_FAILURE);
	};

	if (value == END){
		printf("ending condition met - last read value is %ld\n",value);
		exit(0);
	}

	sem_post(&(sem_free_slot[my_index]));

	my_index = (my_index+1)%SIZE;
		
	data++;

	goto retry;

}

int main(int argc, char** argv){

	int prod, cons;
	int i;

	sem_free_slot = (sem_t*)mmap(NULL,SIZE*sizeof(sem_t),PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,0,0);
        if (sem_free_slot == NULL){
                printf("mmap error on free slot semaphores\n");
                exit(EXIT_FAILURE);

        }       

	for (i=0; i<SIZE; i++){
		if(sem_init(&(sem_free_slot[i]),1,1) == -1){
			printf("cannot init free slot semaphore at iteration %d\n",i);
			exit(EXIT_FAILURE);
		}	

	}

	sem_available_item = (sem_t*)mmap(NULL,SIZE*sizeof(sem_t),PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,0,0);
        if (sem_available_item == NULL){
                printf("mmap error on available item semaphores\n");
                exit(EXIT_FAILURE);

        }       
	for (i=0; i<SIZE; i++){
		if(sem_init(&(sem_available_item[i]),1,0) == -1){
			printf("cannot init available item semaphores at iteration %d\n",i);
			exit(EXIT_FAILURE);
		}
	}

	v = (long*)mmap(NULL,SIZE*sizeof(long),PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,0,0);
        if (v == NULL){
                printf("mmap error\n");
                exit(EXIT_FAILURE);

        }       
	
	prod = fork();
	if (prod == -1){
		printf("fork on producer error\n");
		exit(EXIT_FAILURE);

	}	

	if (prod == 0){
		producer();
	}


	cons = fork();
	if (cons == -1){
		printf("fork on consumer error\n");
		exit(EXIT_FAILURE);

	}	

	if (cons == 0){
		consumer();
	}

	wait(NULL);
	exit(0);

}

