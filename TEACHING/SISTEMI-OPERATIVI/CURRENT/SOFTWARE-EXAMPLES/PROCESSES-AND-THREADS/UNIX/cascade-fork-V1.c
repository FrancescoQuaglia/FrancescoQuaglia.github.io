#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_FORKS 10

int main(int a, char ** b){

	int residual_forks = NUM_FORKS;

	another_fork:

	while(residual_forks-- > 0){
		if(fork()>0){
			break;
		}
		else{
			continue;
		}
	}
	printf("process %d starts pausing\n",getpid());
	pause();



}
