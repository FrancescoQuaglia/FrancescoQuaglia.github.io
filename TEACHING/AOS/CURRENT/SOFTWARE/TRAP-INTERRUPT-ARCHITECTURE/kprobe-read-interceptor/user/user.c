#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main (int a, char** b){

	char buff[64];

	printf("address is %p\n",buff);

	while(1){
		write(1,buff,read(0,buff,64));
	}

	return 0;
}
