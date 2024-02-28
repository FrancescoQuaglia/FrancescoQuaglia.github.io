// mmaps an anonymous or non-anonymous shared page depending on the FILE_MAP macro

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define PAGE_SIZE 4096

int gets(char*);

int main(int argc, char** argv){

 void*addr, *oldboundary;
 int i;
 int fd;

 addr = oldboundary = (void*)sbrk(0);
// printf("old program boundary is at address %x (boundary page number is %d)\n",(unsigned)addr, (unsigned)addr/(unsigned)PAGE_SIZE);



#ifndef FILE_MAP
 mmap(addr,PAGE_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS|MAP_FIXED,fd,0);
 sprintf(addr,"ciao");
#else
 if(!argv[1]){
	printf("file name not supplied\n");
	exit(EXIT_FAILURE);
 }
 fd = open(argv[1],O_RDWR);
 addr =  mmap(0,PAGE_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
#endif

 if (addr == NULL){
	printf("mmap failure\n");
	exit(EXIT_FAILURE);
 }


 if(fork()){
	while(1)
	gets((char*)addr);
 }
 else{
	i = 0;
	while(1){
		sleep(2);
		printf("round %d - buffer content is: %s\n",i++,(char*)addr);
		fflush(stdout);
	}
 }
}

