#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>


void printA(){
	printf("A\n");
}

void printB(){
	printf("B\n");
}

void *generic_threadA(){
	signal(SIGINT,printA);
	while(1) {
		pause();
		printf("I'm thread A\n");
	}
}

void *generic_threadB(){
	signal(SIGINT,printB);
	while(1) {
		pause();
		printf("I'm thread B\n");
	}
}



int main(int argc, char **argv){

  pthread_t thread;
  pthread_attr_t attr;

  pthread_create(&thread, NULL, generic_threadA, NULL);
  pthread_create(&thread, NULL, generic_threadB, NULL);


  while(1) {
	pause();
	printf("I'm thread main\n");
  }



}
