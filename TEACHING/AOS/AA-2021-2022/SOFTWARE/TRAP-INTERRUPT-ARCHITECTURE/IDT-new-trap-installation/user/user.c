#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifndef SLEEP_TIME
#define SLEEP_TIME (1000000)
#endif

char* message = "awake - traping to kernel\n";

int main(int x, char** y){
redo:
	usleep(SLEEP_TIME);
	write(1,message,strlen(message));
	asm volatile("int $0xee":::);
	goto redo;
	exit(0);
}

