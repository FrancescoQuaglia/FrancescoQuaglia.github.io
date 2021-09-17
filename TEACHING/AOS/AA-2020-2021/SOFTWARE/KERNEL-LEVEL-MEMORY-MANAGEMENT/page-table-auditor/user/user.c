#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int audit_map(int service_code, int level){
	return syscall(service_code,level);
}


int main(int argc, char** argv){

	int syscall;
	int level;

	if(argc < 3){
		printf("expected command line: prog syscall_num syscall_param\n");
		return EXIT_FAILURE;
	}
	syscall = strtol(argv[1],NULL,10);
	level = strtol(argv[2],NULL,10);
        
	audit_map(syscall,level);
	
	return 0;

}

