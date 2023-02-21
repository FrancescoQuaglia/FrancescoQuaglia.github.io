#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include<sys/wait.h>

#define PARAMS // this allows running commands with paramenters, but still limited to foreground execution

#define MAX_ARGS 128

#define AUDIT 

#define LINE_SIZE (4096)

char* s = " \n";//the \n is because we are reading input data with fgets()

int main(int argc, char** argv) {
      char command_line[LINE_SIZE];
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
	  fgets(command_line,LINE_SIZE,stdin);
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
              exit(1);
          }
          if ( pid == 0 ){
#ifndef PARAMS
		execlp(command_line,command_line,0);
#else
		printf("try to run %s",args[0]);
		fflush(stdout);
		execvp(args[0],args);
#endif
	 	printf("Unable to run the typed command\n");	
	  }
          else wait(&status);
      }
}

