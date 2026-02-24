#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include <errno.h>

int main(int a, char **b){
	
 	pid_t pid, ppid;;
	int exit_code;

	if (a < 2){
		printf("invalid number of arguments\n");
		exit(EXIT_FAILURE);
	}
	pid = fork();

	if (pid == 0){

		ppid = strtol(b[1],NULL,10);
		printf("reference process id is %d\n",ppid);
	redo:
		printf("\nchild process querying parent id is %d\n",getppid());
		if (getppid() == ppid){
		       printf("now my parent is systemd - I'm exiting\n");
	       	       exit(0);
		}
		printf("going to sleep\n");
		fflush(stdout);
		sleep(3);
		goto redo;
	}

	sleep(1);
	exit(0);

}
