#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

#define PAGE_SIZE (4096) 

#define NUM_TARGET_PAGES 50000

char v[PAGE_SIZE*NUM_TARGET_PAGES] __attribute__((aligned(4096)));

int main(int argc, char** arv){
        
	int i;
	char* buffer;

	buffer = (char*)mmap(NULL,PAGE_SIZE*NUM_TARGET_PAGES,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,0,0);

 	buffer = v;

#ifndef SAME_PAGE_WRITE
	for (i=0;i<NUM_TARGET_PAGES;i++){
		buffer[i*PAGE_SIZE] = 'f';
	}
#else
	for (i=0;i<NUM_TARGET_PAGES;i++){
		buffer[0] = 'f';
	}
#endif


}
