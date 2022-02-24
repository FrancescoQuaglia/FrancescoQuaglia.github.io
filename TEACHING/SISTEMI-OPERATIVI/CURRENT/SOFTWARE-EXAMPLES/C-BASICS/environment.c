
//please compile with -nostartfiles

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

extern char** environ;

void _start(void){
   	 char ** addr = environ;

         printf("environ head pointer is at address: %u\n",(unsigned long)environ);
	 
	 while(*addr){
      	      printf("%s\n",*(addr));
 	      addr++;
	}
	exit(0);

}
