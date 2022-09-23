#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define CODE 134 //this depends on what the kernel tells you when mounting the printk-example module


int main(int argc, char** argv){
       
       	syscall(CODE);		
	return 0;
}

