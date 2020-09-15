
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct desc_ptr {
	unsigned short size;
	unsigned long address;
} __attribute__((packed)) ;

#define store_gdt(ptr) asm volatile("sgdt %0":"=m"(*ptr))

int main (int argc, char**argv){

	struct desc_ptr gdtptr;	
	char v[10];//another way to see 10 bytes packed in memory
	
	store_gdt(&gdtptr);
	store_gdt(v);

	printf("comparison is %d\n",memcmp(v,&gdtptr,10));

	printf("GDTR is at %p - size is %d\n",gdtptr.address, gdtptr.size); 

	printf("GDTR is at %p - size is %d\n",((struct desc_ptr*)v)->address,((struct desc_ptr*)v)->size);

}

