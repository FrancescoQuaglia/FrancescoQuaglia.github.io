#include <stdio.h>
#include <fcntl.h>
#include <signal.h>

unsigned int a,b,c,d,e,f;

void printdata(int sgnumber){
	printf("values are %d - %d - %d -%d -%d - %d \n",a,b,c,d,e,f);
}

int main (int argc, char**argv){

	unsigned int i = 0;
	signal(SIGINT, printdata);

	while (1){
		a = b = c = d = e = f = (++i)%1024; 	
	}	

}
