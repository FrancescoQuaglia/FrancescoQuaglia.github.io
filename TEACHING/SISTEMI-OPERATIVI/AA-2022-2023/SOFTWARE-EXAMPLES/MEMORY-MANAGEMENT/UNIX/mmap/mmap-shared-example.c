#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PAGE_SIZE (4096) 

#define NUM_TARGET_PAGES 10000

int gets(char*);

int main(int argc, char** arv){
        
	pid_t pid;
	char* buffer;
        

	buffer = (char*)mmap(NULL,PAGE_SIZE*NUM_TARGET_PAGES,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,0,0);
	if (buffer == NULL){
		printf("mmap error\n");
		return 1;

	}	

	pid = fork();

	if(pid == -1){
		printf("fork error\n");	
		return 2;

	}

	if(pid == 0){
		printf("give me a string:\n");
		gets(buffer);
		return 0;
	}	
	
	wait(NULL);

	printf("%s\n",buffer);

	return 0;


}
