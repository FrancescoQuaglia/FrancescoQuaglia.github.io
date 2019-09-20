#include <stdlib.h>
#include <stdio.h>

#define AM 134 //this depends on what the kernel tells you when mounting the vtpmo module

int audit_map(void){
	return syscall(AM);
}


int main(int argc, char** argv){
        
	audit_map();

	pause();
	
	return 0;

}

