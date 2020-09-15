#include <stdio.h>
#include <unistd.h>

int main (int a, char** b){

	char buff[64];

	while(1){
		write(1,buff,read(0,buff,64));
	}

	return 0;
}
