#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<unistd.h>


void* child_thread(void*p){

	again:
	sleep(1);
	printf("I'm alive\n");
	goto again;
}

int main(int argc, char** argv){

   pthread_t tid;

   pthread_create(&tid,NULL,child_thread,NULL);

   syscall(60,0);

}
