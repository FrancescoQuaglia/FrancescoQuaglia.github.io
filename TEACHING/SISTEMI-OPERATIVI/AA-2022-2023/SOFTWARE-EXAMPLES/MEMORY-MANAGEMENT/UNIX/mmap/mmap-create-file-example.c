#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#define PAGE_SIZE (4096) 

int gets(char*);

int main(int argc, char** argv){
        
	pid_t pid;
	int fd;
	char* buffer;

	if (argc!= 2) {
    		printf("Syntax: prog file_name\n");
   	 	return 1;
  	}


  	fd=open(argv[1], O_CREAT|O_RDWR, 0660);
	
	if (fd== -1) {
   	 printf("open error\n");
  	 return 2; 
	}
       

	buffer = (char*)mmap(NULL,PAGE_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	if (buffer == NULL){
		printf("mmap error\n");
		return 3;

	}	

	pid = fork();

	if(pid == -1){
		printf("fork error\n");	
		return 4;

	}

	if(pid == 0){
		gets(buffer);		
		return 0;
	}	
	
	wait(NULL);

	printf("%s\n",buffer);

	return 0;


}
