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

	 if (argc < 3) { /* check the number of arguments */ 

		printf("usage: extract archive file\n");
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

	  if(strcmp(argv[2], tar_head.file_names[i])==0){
	    printf("File %s - found in the archive\n",argv[2]);
	    fflush(stdout);
	    file_start = strtoul(tar_head.start_position[i].pos,NULL,10);
	    file_end = strtoul(tar_head.end_position[i].pos,NULL,10);
	    printf("File %s - found in the archive - position is [%ld,%ld]\n",argv[2],file_start,file_end);
	 
	    seek = lseek(dd,file_start,SEEK_SET);

	    printf("seek position for file %s is %ld\n",argv[2],seek);
	    fflush(stdout);

	//please optimize here and put the possibility to extract to a file rather than stdout
	    for(;seek<file_end;seek++){
		read(dd,&c,1);
		write(1,&c,1);

	    }
		

	   return 0;
	  }	
	}

	printf("file %s not found in the archive\n",argv[2]);
	fflush(stdout);

  }/* end main*/

