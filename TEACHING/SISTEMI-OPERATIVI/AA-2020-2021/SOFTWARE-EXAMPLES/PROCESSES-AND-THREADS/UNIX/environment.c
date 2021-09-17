#include <unistd.h>
#include <stdio.h>

extern char** environ;


void main(int argc, char **argv){
   	 char ** addr=environ;

         printf("process %d - environ head pointer is at address: %u\n",getpid(),(unsigned long)environ);
	 fflush(stdout);
	 
	 while(*addr){
      	      printf("%s\n",*(addr));
	      fflush(stdout);
 	      addr++;
	}

  //      execve(argv[0],argv,NULL);
 //       execve(argv[0],argv,environ);
//        execve(argv[0],NULL,NULL);
        execve("/usr/bin/ls",NULL,environ);
	printf("execve failed\n");
	fflush(stdout);
}


