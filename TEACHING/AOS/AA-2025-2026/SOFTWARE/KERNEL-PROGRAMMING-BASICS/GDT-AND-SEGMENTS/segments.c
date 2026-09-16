
#include <stdio.h>


#define load(ptr,var) asm volatile("mov %%cs:(%0), %%rax":"=a" (var):"a" (ptr)) //please try other segment selectors!!

#define store(val,ptr) asm volatile("mov %0, %%ds:(%1)\n"::"a" (val), "b" (ptr):)

int main (int argc, char**argv){

	unsigned long x = 16;

	unsigned long y;

	load(&x,y);
	printf("variable y has value %lu\n",y);

	store(y+1,&x);	
	printf("variable x has value %lu\n",x);

}

