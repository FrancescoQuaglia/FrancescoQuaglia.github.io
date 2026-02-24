#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int a, char **b){
	
 	pid_t pid;

	pid = fork();

	if (pid == 0){
		printf("child process simply exiting\n");
		exit(0);
	}

	printf("parent process simply goes pausing\n");
	pause();

}
