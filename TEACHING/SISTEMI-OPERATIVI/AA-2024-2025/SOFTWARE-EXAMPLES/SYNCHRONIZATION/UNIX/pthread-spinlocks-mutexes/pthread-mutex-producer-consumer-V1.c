#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


#define SIZE (100000)
#define END (10000000)

#define AUDIT if(0)

long v[SIZE] = {[0 ... (SIZE-1)] -1};
pthread_mutex_t available_data[SIZE];
pthread_mutex_t available_slot[SIZE];


void * producer(void* dummy){

	long data = 0;
	long my_index = 0;

	printf("ready to produce\n");

retry:
	pthread_mutex_lock(&available_slot[my_index]);
	
	v[my_index] = data;

	pthread_mutex_unlock(&available_data[my_index]); //set the data available mutex to available 

	my_index = (my_index+1)%SIZE;
	data++;

	goto retry;


}

void * consumer(void* dummy){

	long data = 0;
	long my_index = 0;
	long value;

	printf("ready to consume\n");

retry:
	pthread_mutex_lock(&available_data[my_index]);

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

	pthread_mutex_unlock(&available_slot[my_index]);//set the available slot mutex to avilable 

	my_index = (my_index+1)%SIZE;
	data++;
	
	goto retry;

}

int main(int argc, char** argv){

	pthread_t prod, cons;
	int i;

	for (i=0;i<SIZE;i++){
		pthread_mutex_init(&available_slot[i],NULL);
		pthread_mutex_init(&available_data[i],NULL);
		pthread_mutex_lock(&available_data[i]);
	}

	pthread_create(&cons,NULL,consumer,NULL);
	pthread_create(&prod,NULL,producer,NULL);

	pause();

}

