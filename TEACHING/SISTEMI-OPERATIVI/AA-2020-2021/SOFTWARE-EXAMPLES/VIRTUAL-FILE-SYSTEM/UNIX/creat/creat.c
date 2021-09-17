#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#define BUFSIZE 1024

int main(int argc, char *argv[]) { 
	int des; 

	 if (argc != 2) { /* check the number of arguments */ 

		printf("usage: creat file-name\n");
		exit(1);
	 }  

	des = creat(argv[1],0660);

	printf("file creation returned with descriptor %d\n",des);

  }/* end main*/

