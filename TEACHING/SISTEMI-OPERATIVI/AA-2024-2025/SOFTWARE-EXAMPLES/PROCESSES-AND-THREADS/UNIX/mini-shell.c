#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include<sys/wait.h>

//#define PARAMS // this allows running commands with paramenters, but still limited to foreground execution

#define MAX_ARGS 128

//#define AUDIT 

int gets(char*);

char* s = " ";
 

int main(int argc, char** argv) {
      char command_line[4096];
      char* p;
      char* args[MAX_ARGS];
      int  pid, status;
      int i;

      printf("Welcome to mini-shell\n");

      while(1) {

          printf("Type a command line: ");

#ifndef PARAMS
          scanf("%s",command_line);
#else
	  gets(command_line);
	  p = (char*)strtok(command_line,s);
	  i = 0;
	  args[i] = p;

	  while(p){
#ifdef AUDIT
		printf("%s " ,p);
#endif
	        fflush(stdout);
	  	p = (char*)strtok(NULL,s);
		args[++i] = p;
		
	  }
	  args[++i] = NULL;

	  printf("\n");
#endif
          pid = fork();
          if ( pid == -1 ) {
              printf("Unable to spawn new process\n");
              exit(EXIT_FAILURE);
          }
          if ( pid == 0 ){
#ifndef PARAMS
		execlp(command_line,command_line,0);
#else
		execvp(args[0],args);
#endif
	 	printf("Unable to run the typed command\n");	
	  }
          else wait(&status);
      }
}

