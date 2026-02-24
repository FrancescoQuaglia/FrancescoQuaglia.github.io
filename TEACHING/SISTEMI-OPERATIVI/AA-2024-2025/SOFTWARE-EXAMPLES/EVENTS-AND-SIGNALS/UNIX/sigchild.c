#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int a, char **b){

	int i;
	
#ifdef IGNORE
	signal(SIGCHLD,SIG_IGN);
#endif

	for (i=0; i<10; i++){
		if(fork() == 0) exit(0);

	}

	pause();

}
