
/******************************************************************
 this program can be exploited to get the difference in performance 
 between x86 fast and slow system call paths
 please redirect the output to /dev/null for more accurate performance data
******************************************************************/

#include <stdlib.h>

char c = 'f'; //putting c here (low addresses) allows correct runs when using ecx as pointer to c with -D m32 compilation


#define CYCLES 10000000


void out(char*);

int _start(int argc){

 int i = 0;

 while(i++<CYCLES){
  out(&c);
 }

 exit(0);

}


void out(char *s){
#ifndef m32
 asm("mov %%rbx, %%rdi ; mov %%rcx , %%rsi ; syscall " :  : "a" (1) , "b" (1) , "c" ((unsigned long)(s)) , "d" ((long)(1)) );
#else
 asm("int $0x80" :  : "a" (4) , "b" (1) , "c" ((unsigned long)(s)) , "d" ((long)(1)) );
#endif
}

