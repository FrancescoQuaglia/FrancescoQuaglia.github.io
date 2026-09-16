#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>


long val = 0;

int main(int argc, char** argv){
	
	char *p;
	unsigned int sleep_time = 0;

	p = (char*)mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0,0);

//	printf("I'm process %d - target addr is: %lu - mmap address is %lu\n",getpid(),&val,p);

	system("clear");

	while(1){
		
		sleep(sleep_time);
		system("clear");
	printf("I'm process %d - target addr is: %lu - mmap address is %lu\n",getpid(),&val,p);
		printf("value is: %ld\n",val);
		sleep_time = 5;
		if (val == 1024) break;

	}

}
