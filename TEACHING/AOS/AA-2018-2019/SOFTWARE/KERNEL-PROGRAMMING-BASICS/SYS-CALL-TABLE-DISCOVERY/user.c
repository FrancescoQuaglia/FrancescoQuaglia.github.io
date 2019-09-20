#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv){
	
	int sys_call_num, arg;
	
	if(argc < 2){
                printf("usage: prog syscall-num [syscall-param]\n");
                return;
        }
        
        
        sys_call_num = strtol(argv[1],NULL,10);
        if (argv[2]){
        	arg = strtol(argv[2],NULL,10);
		syscall(sys_call_num,arg);
		return 0;
        }
	
	syscall(sys_call_num);
	
	return 0;
}
	
