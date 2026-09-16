#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>

unsigned long MEM_SIZE = 512*4096;

int main(int argc, char** argv){

	char* base_ptr;

	base_ptr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);

	printf("mmap returned %p\n",base_ptr);
	
	base_ptr[0] = 'f';
	
	pause();

}
