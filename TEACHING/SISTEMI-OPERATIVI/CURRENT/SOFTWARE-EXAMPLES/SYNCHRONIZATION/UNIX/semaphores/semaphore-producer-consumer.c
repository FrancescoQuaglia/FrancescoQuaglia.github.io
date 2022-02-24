#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>


#define SIZE (100)
#define END (1000000)

#define AUDIT if(0)

long *v;

int sem_free_slot, sem_available_item;

void * producer(void){

	struct sembuf oper;

	long data = 0;
	long my_index = 0;

	printf("ready to produce\n");

retry:
	oper.sem_num = my_index;
	oper.sem_op = -1;
	oper.sem_flg = 0;

	semop(sem_free_slot,&oper,1);
	
	v[my_index] = data;

	oper.sem_num = my_index;
	oper.sem_op = 1;
	oper.sem_flg = 0;

	semop(sem_available_item,&oper,1);

	my_index = (my_index+1)%SIZE;
	data++;

	goto retry;


}

void * consumer(void){

	long data = 0;
	long my_index = 0;
	long value;
	struct sembuf oper;

	printf("ready to consume\n");

retry:
	oper.sem_num = my_index;
	oper.sem_op = -1;
	oper.sem_flg = 0;

	semop(sem_available_item,&oper,1);
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

	oper.sem_num = my_index;
	oper.sem_op = 1;
	oper.sem_flg = 0;

	semop(sem_free_slot,&oper,1);

	my_index = (my_index+1)%SIZE;
		
	data++;

	goto retry;

}

int main(int argc, char** argv){

	int prod, cons;
	int i;
	key_t key = IPC_PRIVATE;

	sem_free_slot = semget(key,SIZE,IPC_CREAT|0666);
	if(sem_free_slot == -1){
		printf("semget error\n");
		exit(-1);
	}

	for (i=0; i<SIZE; i++){
	
		semctl(sem_free_slot,i,SETVAL,1);

	}

	sem_available_item = semget(key,SIZE,IPC_CREAT|0666);
	if(sem_available_item == -1){
		printf("semget error\n");
		exit(-1);
	}

	for (i=0; i<SIZE; i++){
		semctl(sem_available_item,i,SETVAL,0);

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

	semctl(sem_free_slot,IPC_RMID,0);
	semctl(sem_available_item,IPC_RMID,0);

	exit(0);

}

