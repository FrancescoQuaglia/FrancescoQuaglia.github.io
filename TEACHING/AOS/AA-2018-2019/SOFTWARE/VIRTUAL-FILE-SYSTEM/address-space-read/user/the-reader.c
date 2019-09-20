#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

char the_command[4096];
char *the_device;

int main(int argc, char** argv){
	
	unsigned long long initial_page;
	unsigned long long final_page;
	long pid;
	unsigned long i;
	unsigned long target_address;
	
	if (argc < 5){
		printf("usage: progname dev-name PID initial-page final-page\n");
		return -1;
	}

	for(i=0;i<argc;i++) printf("%s\n",argv[i]);

	the_device = argv[1];
	pid = strtol(argv[2],NULL,10);	
	initial_page = strtoull(argv[3],NULL,10);	
	final_page = strtoull(argv[4],NULL,10);	

	if (final_page < initial_page){ 
		printf("no page range to query\n");
		return -1;
	}
	
	printf("querying from page %llu to page %llu\n",initial_page,final_page);
	
	for(i=initial_page;i<=final_page;i++){
		target_address = ((unsigned long)i)*4096;
		printf("----------------------------------\n");
		printf("attack adress is %lu\n",target_address);
		sprintf(the_command,"echo '%d %lu' > %s\n",pid,target_address,the_device); 
		printf("running the command: %s", the_command);
		system(the_command);
		sprintf(the_command,"cat %s\n",the_device); 
		printf("running the command: %s", the_command);
		system(the_command);
		printf("\n");

	}
	

}
