#include <stdio.h>


void update(unsigned long *ptr,unsigned long var){ 
	asm volatile("mov %%rsi, %%ds:(%%rdi)":::);
}
	

int main (int argc, char**argv){

	unsigned long x = 16;

	unsigned long y;

	update(&y,x);
	printf("variable y has value %u\n",y);


}
