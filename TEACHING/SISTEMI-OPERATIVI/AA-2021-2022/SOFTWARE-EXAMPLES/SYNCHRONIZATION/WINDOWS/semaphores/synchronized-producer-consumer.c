#include <windows.h>
#include <stdio.h>
#include <stdlib.h>



#define SIZE (100)
#define END (1000000)

#define AUDIT if(0)

long v[SIZE];

long counter = 0;

HANDLE sem_free_slot[SIZE];

HANDLE sem_available_item[SIZE];


DWORD producer(void){

	long data = 0;
	long my_index = 0;
	long dummy;

	printf("ready to produce\n");
	fflush(stdout);

retry:
	WaitForSingleObject(sem_free_slot[my_index], INFINITE);

	v[my_index] = data;

	ReleaseSemaphore(sem_available_item[my_index],1,&dummy);
	my_index = (my_index + 1) % SIZE;
	data++;
			counter++;


	goto retry;

	return 0;

}

WORD consumer(void){

	long data = 0;
	long my_index = 0;
	long value;
	long dummy;

	printf("ready to consume\n");
	fflush(stdout);

retry:
	WaitForSingleObject(sem_available_item[my_index], INFINITE);

	value = v[my_index];

	AUDIT
	printf("consumer got value %d\n", value);

	if (value != data){
			printf("consumer: synch protocol broken at expected value: %d - real is %d!!\n", data, value);
			ExitProcess(-1);
	};

	if (value == END){
			printf("ending condition met - last read value is %d\n", value);
			ExitProcess(0);
	}

	ReleaseSemaphore(sem_free_slot[my_index],1,&dummy);

	my_index = (my_index + 1) % SIZE;

	data++;
	counter--;

goto retry;

}



int main(int argc, char *argv[]) {

	HANDLE hProducerThread;
	HANDLE hConsumerThread;

	DWORD hid;
	DWORD exit_code;
	int i;
	char buffer[128];//mutex names

	for (i = 0; i < SIZE; i++){
		sprintf(buffer, "slots%d", i);
		sem_free_slot[i] = CreateSemaphore(NULL, 1,1, buffer);
		sprintf(buffer, "items%d", i);
		sem_available_item[i] = CreateSemaphore(NULL, 0,1, buffer);

	}

	hConsumerThread = CreateThread(NULL,
		0,
		(LPTHREAD_START_ROUTINE)consumer,
		NULL,
		NORMAL_PRIORITY_CLASS,
		&hid);

	hProducerThread = CreateThread(NULL,
		0,
		(LPTHREAD_START_ROUTINE)producer,
		NULL,
		NORMAL_PRIORITY_CLASS,
		&hid);


	WaitForSingleObject(hConsumerThread, INFINITE);
	WaitForSingleObject(hProducerThread, INFINITE);

}


