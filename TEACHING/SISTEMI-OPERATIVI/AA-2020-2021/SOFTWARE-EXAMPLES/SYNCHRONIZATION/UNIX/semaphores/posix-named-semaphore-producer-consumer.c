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

sem_t *sem_free_slot[SIZE], *sem_available_item[SIZE];

void * producer(void){

	long data = 0;
	long my_index = 0;

	printf("ready to produce\n");

retry:

	sem_wait(sem_free_slot[my_index]);
	
	v[my_index] = data;

	sem_post(sem_available_item[my_index]);

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
	sem_wait(sem_available_item[my_index]);
	value = v[my_index];

	AUDIT
	printf("consumer got value %d\n",value);

	if(value != data){
		printf("consumer: synch protocol broken at expected value: %d - real is %d!!\n",data+1,value);
		exit(-1);
	};

	if (value == END){
		printf("ending condition met - last read value is %d\n",value);
		exit(0);
	}

	sem_post(sem_free_slot[my_index]);

	my_index = (my_index+1)%SIZE;
		
	data++;

	goto retry;

}

int main(int argc, char** argv){

	int prod, cons;
	int i;
	char buff[128];

	for (i=0; i<SIZE; i++){
		sprintf(buff,"slot%d\0",i);
		sem_free_slot[i] = sem_open(buff,O_CREAT,0666,1);
		if (sem_free_slot[i] == NULL){
			printf("cannot create free slot semaphore at iteration %d\n",i);
			exit(0);
		}	
		sem_unlink(buff);

	}

	for (i=0; i<SIZE; i++){
		sprintf(buff,"item%d\0",i);
		sem_available_item[i] = sem_open(buff,O_CREAT,0666,0);
		if (sem_available_item[i] == NULL){
			printf("cannot create available slot semaphore at iteration %d\n",i);
			exit(0);
		}	
		sem_unlink(buff);

	}

	v = (long*)mmap(NULL,SIZE*sizeof(long),PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,0,0);
        if (v == NULL){
                printf("mmap error\n");
                exit(-1);

        }       
	
	prod = fork();
	if (prod == -1){
		printf("fork on producer error\n");
		exit(-1);

	}	

	if (prod == 0){
		producer();
	}


	cons = fork();
	if (cons == -1){
		printf("fork on consumer error\n");
		exit(-1);

	}	

	if (cons == 0){
		consumer();
	}

	wait(NULL);
	exit(0);

}

