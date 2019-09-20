#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

_start(){
	sleep(2);
	write(1,"awake\n",6);
	asm volatile("mov %%rax,%%r15 ; int $0xf5"::"a" (_start):);
	exit(0);
}

