#include <unistd.h>
#include <stdlib.h>

#define NUM_FORKS 10

int main(int a, char ** b){

	int residual_forks = NUM_FORKS;

	for(;residual_forks > 0 ; residual_forks--){
		if(fork()>0){
				continue;
		}
		else{
			break;
		}
	

	}
	pause();



}
