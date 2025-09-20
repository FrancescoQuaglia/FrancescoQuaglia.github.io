#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef FAULT
char *buffer = NULL;
#else
char buffer[4096];
#endif

int main(int argc, char** argv){
	
	int sys_call_num, arg;
	int ret;
	
	if(argc < 2){
                printf("usage: prog syscall-num [syscall-param]\n");
                return EXIT_FAILURE;
        }
        
        
        sys_call_num = strtol(argv[1],NULL,10);

        if (argv[2]){
		syscall(sys_call_num,argv[2],strlen(argv[2]));
		return 0;
        }
	
	ret = syscall(sys_call_num,buffer,4096);

	if (ret == -1) return  EXIT_FAILURE;

	if (ret == 0){
	       printf("no message delivered\n");
       	       return  EXIT_FAILURE;
	}

	buffer[ret] = '\0';
	
	printf("got message: %s\n",buffer);

	return 0;
}
	
