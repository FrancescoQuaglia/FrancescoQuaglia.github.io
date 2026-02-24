#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

#define PAGE_SIZE (4096) 

#define NUM_TARGET_PAGES 10 

int main(int argc, char** arv){
        
	char* buffer;
	int ret;
        

	buffer = (char*)mmap(NULL,PAGE_SIZE*NUM_TARGET_PAGES,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,0,0);
	if (buffer == NULL){
		printf("mmap error\n");
		return -1;
	}

	ret = munmap(buffer,PAGE_SIZE);//unmapping the initial page of the map

	if (ret == -1){
		printf("munmap error\n");
		return -1;
	}

	//using the still mapped pages
	scanf("%s",buffer+PAGE_SIZE);
	printf("%s\n",buffer+PAGE_SIZE);

	return 0;

}
