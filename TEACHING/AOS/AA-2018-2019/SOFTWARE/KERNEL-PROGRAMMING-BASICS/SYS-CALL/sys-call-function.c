#include <stdlib.h>

#define SIZE 4096

char buff[SIZE];

int main (int a, char** b){
	while(1){
		syscall(0,0,buff,SIZE);
		syscall(1,1,buff,SIZE);
		syscall(1,1,"\n",1);
	}
}
