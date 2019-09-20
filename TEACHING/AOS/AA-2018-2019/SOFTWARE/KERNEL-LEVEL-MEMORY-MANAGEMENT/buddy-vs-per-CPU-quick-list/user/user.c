#include <stdlib.h>
#include <stdio.h>

#define AUDIT  if(0)

#define CYCLES 10000000

int main(int argc, char** argv){
	
	int get_sys_call_num, release_sys_call_num;
	void* addr;
	int ret;
	int i;
	
	
	if(argc < 3){
                printf("usage: prog get-syscall-num release-syscall-num\n");
                return;
        }
        
        
        get_sys_call_num = strtol(argv[1],NULL,10);
        release_sys_call_num = strtol(argv[2],NULL,10);

	for (i = 0; i<CYCLES;i++){	
		addr = NULL;
		ret = syscall(get_sys_call_num,&addr);
		AUDIT
		printf("get syscall returned value %d - address is %p\n",ret,addr);
	
		syscall(release_sys_call_num,addr);
		AUDIT
		printf("release syscall returned value %d - passed address is %p\n",ret,addr);
	}
	
	return 0;
}
	
