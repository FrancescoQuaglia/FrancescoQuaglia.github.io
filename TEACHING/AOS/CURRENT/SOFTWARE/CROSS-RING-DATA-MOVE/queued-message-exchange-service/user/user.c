#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


char message[4096];

#define NUM_MESSAGES 10

char * buff[NUM_MESSAGES];

int main(int argc, char** argv){
	
	int sys_call_num, arg;
	int ret;
	int i;
	
	if(argc < 2){
                printf("usage: prog syscall-num [nsg-body]\n");
                return EXIT_FAILURE;
        }
        
        
        sys_call_num = strtol(argv[1],NULL,10);

        if (argv[2]){
		for(i=0; i< NUM_MESSAGES; i++){
			buff[i] = malloc(4096);
			sprintf(buff[i],"%s(%d)\n",argv[2],i);
			printf("sending %s",buff[i]);
			ret = syscall(sys_call_num,buff[i],strlen(buff[i]));
		}
		return 0;
        }
	
	for(i=0; i< NUM_MESSAGES; i++){
			ret = syscall(sys_call_num,message,4096-1);
			message[ret] = '\0';
			printf("ret is %d - got message: %s",ret,message);
	}

	return 0;
}
	
