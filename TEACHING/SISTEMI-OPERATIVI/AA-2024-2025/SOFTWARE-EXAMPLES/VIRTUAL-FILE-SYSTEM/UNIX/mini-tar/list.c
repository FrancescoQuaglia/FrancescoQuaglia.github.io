#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "mini-tar.h"


header tar_head;

int main(int argc, char *argv[]) { 
	int dd, size, result; 
	int i;
	off_t seek;
	off_t file_start, file_end;
	char c;

	 if (argc != 2) { /* check the number of arguments */ 

		printf("usage: list archive\n");
		return EXIT_FAILURE;
	 }  


	 /* archive opening */
	 dd=open(argv[1],O_RDONLY); 
	 if (dd == -1) {
		printf("archive open error\n");
		fflush(stdout);
	        return EXIT_FAILURE;
	 }

	result = read(dd,&tar_head,sizeof(tar_head));

	if (result != sizeof(tar_head)){//please make more robust with respect to residuals
	   printf("unable to read header in archive %s\n",argv[1]);
	   fflush(stdout);
	   return EXIT_FAILURE;
	}

	if (strcmp(the_magic,tar_head.magic) != 0){
	   printf("file %s is not a minitar\n",argv[1]);
	   fflush(stdout);
	   return EXIT_FAILURE;
	}

	for (i=0;i<MAX_MANAGEABLE_FILES;i++){

	  if(strcmp("",tar_head.file_names[i]) == 0) return 0;
	  printf("%s\n",tar_head.file_names[i]);
	  fflush(stdout);
	  	
	}

  }/* end main*/

