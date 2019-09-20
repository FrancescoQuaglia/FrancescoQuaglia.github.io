
#include <stdio.h>


#define load(ptr,var) asm volatile("mov %%fs:(%0), %%rax":"=a" (var):"a" (ptr))
#define store(val,ptr) asm volatile("push %%rbx; mov %0, %%ds:(%1); pop %%rbx\n"\
						 ::"a" (val), "b" (ptr):)

int main (int argc, char**argv){

	unsigned long x = 16;

	unsigned long y;

	load(&x,y);
	printf("variable y has value %u\n",y);

	store(y+1,&x);	
	printf("variable x has value %u\n",x);

}

