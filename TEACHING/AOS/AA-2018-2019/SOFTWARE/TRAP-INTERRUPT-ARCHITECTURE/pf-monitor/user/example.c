
/**************************************************************************
   this user program allows you to access the sys_monotor_reset 
   and sys_get_pf system calls added to the kernel via the
   service-installer module of the page fault monitoring architecture
**************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include "log.h"

#define PAGE_SIZE (4096)
#define NUM_PAGES (128)


//char *buff;
char buff[PAGE_SIZE*NUM_PAGES];

#define AUDIT if(1)

int monitor_reset(long int x){
	return syscall(x);
}

int get_pf(long int x, pf_log * p){
	return syscall(x,p);
}


int main(int argc, char** argv){
	
	long int monitor_reset_code;	
	long int pf_get_code;
	pf_log info;
	int j;
	int ret;
	char c;
	char* addr;

	if(argc < 3){
		printf("need argv[1] and argv[2] (the numbers corresponding to the PF monitoring service system-calls (please check with dmesg)\n");
		return -1;
	}

	monitor_reset_code = strtol(argv[1],NULL,10);
	pf_get_code = strtol(argv[2],NULL,10);

	ret = monitor_reset(monitor_reset_code);
	printf("monitor reset service returned %d\n",ret);

	j = 0;	
	addr = buff;
	do{
		addr += j*PAGE_SIZE;	
		j++;
		scanf("%s",addr);
	}while(strcmp(addr,"quit")!=0);	


	ret = get_pf(pf_get_code, &info);
	printf("pf get service returned %d\n",ret);

	printf("PF info: read mode %d - write mode %d\n",info.read,info.write);

}
