#include <stdio.h>

unsigned long y;

void update(unsigned long *ptr, unsigned long var){ 
	asm volatile("mov %%rsi, %%ds:(%%rdi)":::);
}
	
__thread int k = 0;

int main (int argc, char**argv){

	unsigned long x = 16;


	update(&y,x);
	printf("variable y has value %lu\n",y);

	k = 1;

	return 0;

}
