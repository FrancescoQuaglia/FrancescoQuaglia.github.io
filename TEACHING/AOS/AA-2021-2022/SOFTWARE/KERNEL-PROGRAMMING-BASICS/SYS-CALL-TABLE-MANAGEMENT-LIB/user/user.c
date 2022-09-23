#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv){
	
	int sys_call_num, arg;
	
	if(argc < 2){
                printf("usage: prog syscall-num\n");
                return 0;
        }
        
        
        sys_call_num = strtol(argv[1],NULL,10);
	
	syscall(sys_call_num);
	
	return 0;
}
	
