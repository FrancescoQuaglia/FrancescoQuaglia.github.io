#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int a, char **b){

	int i;
	int value;
	
	for (i=0; i<10; i++){
		if(fork() == 0) pause();
	}

	signal(SIGINT,SIG_IGN);

	for (i=0; i<10; i++){
		wait(&value);
		printf("last two bytes of value are %d - %d\n",(value>>8)&255,value&255); 
	}
	

}
