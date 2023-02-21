#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int a, char **b){
	
 	pid_t pid;
	int exit_code;

	pid = fork();

	if (pid == 0){
	redo:
		printf("child process querying parent id is %d\n",getppid());
		if (getppid()==1) exit(0);
		sleep(3);
		goto redo;
	}

	sleep(1);
	exit(0);

}
