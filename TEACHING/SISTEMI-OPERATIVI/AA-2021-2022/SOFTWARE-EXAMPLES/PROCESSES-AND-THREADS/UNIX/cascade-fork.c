#include <unistd.h>
#include <stdlib.h>

#define NUM_FORKS 10

int main(int a, char ** b){

	int residual_forks = NUM_FORKS;

	another_fork:

	residual_forks--;
	if(fork()>0){
		pause();
	}
	else{
		if(residual_forks>0){	;
		 	goto another_fork;	
		}
	}
	pause();



}
