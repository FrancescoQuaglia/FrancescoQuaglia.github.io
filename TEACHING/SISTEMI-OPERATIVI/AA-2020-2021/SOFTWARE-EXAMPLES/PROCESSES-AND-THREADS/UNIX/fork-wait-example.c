#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int a, char **b){
	
 	pid_t pid;
	int exit_code;

	pid = fork();

	if (pid == 0){

		printf("child process simply exiting\n");
		exit(1);
	}

	sleep(7);
	printf("parent process simply goes waiting\n");
	wait(&exit_code);
	printf("parent process: child exited with code %d\n",exit_code>>8);
	pause();

}
