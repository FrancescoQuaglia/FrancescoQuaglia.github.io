/************************************************************************
 example usag oif vecorized C intrinsics
 please compare the execution time with and without 'scaling'
************************************************************************/


#include <emmintrin.h>
#include <string.h>

#define N 1024

#define CYCLES 100000

char a[N] __attribute__ ((aligned (8)));
char b[N] __attribute__ ((aligned (8)));
char c[N] __attribute__ ((aligned (8)));

int main(int argc, char ** argv){

	int i,j;

	//char a[N], b[N], c[N];

	__m64 *av, *bv, *cv;

	av = (__m64*)a; // assume 8-byte aligned
	bv = (__m64*)b; // assume 8-byte aligned
	cv = (__m64*)c; // assume 8-byte aligned

	if(argv[1] && strcmp(argv[1],"scaling")==0){
		for(j = 0 ; j<CYCLES; j++)
			for (i = 0; i < N/8; i++)
  				av[i] = _mm_add_pi8(bv[i], cv[i]);
	}
	else{

		for(j = 0 ; j<CYCLES; j++)
			for (i = 0; i < N; i++)
  				a[i] = b[i] + c[i];
	}
}
