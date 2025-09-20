#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char ** argv){

	int fd;
	long off;
	int ret;
	char * p;
	int to_write;

	 if (argc != 4) { /* check the number of arguments */ 
		printf("usage: prog file-name content offset\n");
		exit(1);
	 }  

	 /* read/write opening of the source file */
	 fd=open(argv[1],O_RDWR);
	 if (fd == -1) {  
		printf("file open error\n");
	        exit(1);
	 }  

	off = strtol(argv[3],NULL,10);

	ret = lseek(fd,off,SEEK_SET);
	if (ret == -1){
		printf("seek error\n");
	        exit(1);
	}

	/* write up to BUFSIZE to destination file */ 
	p = argv[2];
	to_write = strlen(argv[2]);
redo:
	ret = write(fd,p,strlen(p));          	
	if (ret == -1) {
           	printf("destination file write error\n");
	       	exit(1);
        }
	else{
		p += ret;
		to_write -= ret;
		if(to_write > 0) goto redo;
	}
} 
