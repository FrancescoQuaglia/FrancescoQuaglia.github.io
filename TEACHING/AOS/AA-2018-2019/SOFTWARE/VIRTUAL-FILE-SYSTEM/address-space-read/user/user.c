#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

char buff[4096]__attribute__((aligned(4096)));

long val = 0;

int main(int argc, char** argv){
	
	char *p = buff;

	printf("I'm process %d - mmap addr is: %lu (%p) - page number is %lu\n",getpid(),p,p,(unsigned long)p>>12);
	*p = 'f';

	printf("I'm process %d - target addr is: %lu - mmap address is %lu\n",getpid(),&val,p);
	printf("type a string: ");
	scanf("%s",p);
	pause();

}
