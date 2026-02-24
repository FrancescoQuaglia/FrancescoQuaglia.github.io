#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int a, char **b){
	
 	pid_t pid;
	int exit_code;

	pid = fork();

	if (pid == 0){

		sleep(4);
		printf("child process exiting\n");
		exit(1);
	}

	printf("parent process goes waiting\n");
	pid = wait(&exit_code);
	if(pid == -1) {
		printf("wait error\n");
		exit(EXIT_FAILURE);
	}
	printf("parent process: child exited with code %d\n",exit_code>>8);

}
