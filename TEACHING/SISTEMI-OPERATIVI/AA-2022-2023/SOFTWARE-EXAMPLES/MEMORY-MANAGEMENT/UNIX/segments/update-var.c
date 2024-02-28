#include <stdio.h>

unsigned long y;

void update(unsigned long *ptr, unsigned long var){ 
	asm volatile("mov %%rsi, %%ds:(%%rdi)":::);
}
	

int main (int argc, char**argv){

	unsigned long x = 16;


	update(&y,x);
	printf("variable y has value %u\n",y);

	return 0;

}
