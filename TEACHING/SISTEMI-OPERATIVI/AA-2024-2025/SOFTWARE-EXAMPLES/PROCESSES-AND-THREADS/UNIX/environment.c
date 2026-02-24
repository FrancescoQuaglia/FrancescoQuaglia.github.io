#include <unistd.h>
#include <stdio.h>

extern char** environ;


void main(int argc, char **argv){
   	 char ** addr=environ;
	 char * const * p = NULL;


         printf("process %d - environ head pointer is at address: %lu\n",getpid(),(unsigned long)environ);
	 fflush(stdout);
	 
	 while(*addr){
      	      printf("%s\n",*(addr));
	      fflush(stdout);
 	      addr++;
	}


//      execve(argv[0],argv,NULL);
//     execve(argv[0],argv,environ);
//      execve(argv[0],p,NULL);
//      execve("/usr/bin/ls",p,environ);
//      execve("/usr/bin/ls",p,NULL);
 
	argv[0] = "/usr/bin/ls";
	argv[1] = NULL;
 	execve("/usr/bin/ls",argv,environ);
	printf("execve failed\n");
	fflush(stdout);
}


