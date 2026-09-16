/**********************************************************************
 example effects of software structuring on pipeline performance
 please try the different variants of pointer-based assignment to memory 

 compile with -nostartfiles
***********************************************************************/


#include <stdlib.h>
#define CYCLES 100000


char v[CYCLES + 1];
char *buff;
char c;
 
void _start(){
	int i,j;

	for(j=0;j<CYCLES/5;j++){
		buff = v;
		for (i=0;i<CYCLES;i++){
			/* please try the two alternatives below */
#ifdef DEPENDENCY
			c = *++buff;
#else
			c = *buff++;
#endif

		}
	}

	exit(0);

}
