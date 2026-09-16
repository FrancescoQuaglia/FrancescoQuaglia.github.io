/****************************************************
 code snippet for assessing loop unroll performance
 please try the different targetes with the makefile
****************************************************/

#include <stdlib.h>

#define CYCLES (1<<24) 

long long int a[CYCLES];
long long int b[CYCLES];
long long int c[CYCLES];

#pragma GCC push_options
#pragma GCC optimize ("unroll-loops")


void _start(void){
	int i,j;
for(j=0;j<(CYCLES>>16);j++){
	for (i=0; i<CYCLES; i++) {
	  a[i] = b[i] * c[i]; 
	}
}
exit(0);
}

#pragma GCC pop_options
