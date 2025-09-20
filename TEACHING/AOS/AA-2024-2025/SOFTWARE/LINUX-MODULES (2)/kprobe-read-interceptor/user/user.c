#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main (int a, char** b){

	char buff[64];
	int i = 0;

	printf("address is %p\n",buff);

	while(i < 3){
		write(1,buff,read(0,buff,64));
		i++;
	}

	return 0;
}
