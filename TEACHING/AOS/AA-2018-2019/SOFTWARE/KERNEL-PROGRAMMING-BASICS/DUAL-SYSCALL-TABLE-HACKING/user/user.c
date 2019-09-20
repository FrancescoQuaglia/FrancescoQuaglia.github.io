#include <stdlib.h>
#include <stdio.h>

#define CYCLES 10000000
//#define CYCLES 10

int main(int argc, char** argv){
	
	long sys_call_num, arg;
	int i;
	
	if(argc < 2){
                printf("usage: prog syscall-num [syscall-param]\n");
                return;
        }
        
        
        sys_call_num = strtol(argv[1],NULL,10);
	
	for(i=0;i<CYCLES;i++){
#ifdef m32
	asm ("int $0x80":: "a" (sys_call_num) );
#else
	syscall(sys_call_num);
#endif
	}
	
	return 0;
}
	
