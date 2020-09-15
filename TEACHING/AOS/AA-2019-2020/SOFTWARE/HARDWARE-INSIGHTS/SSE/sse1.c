/***************************************************************
 example of usage of vectorized C intrinsics
 please assess the execution time with and without 'scaling'
****************************************************************/

#include <emmintrin.h>
#include <stdio.h>

#define N 128

#define CYCLES 5000000

int a[N] __attribute__ ((aligned (128)));
int b[N] __attribute__ ((aligned (128)));
int c[N] __attribute__ ((aligned (128)));

int main(int argc, char ** argv){

	int i,j;

	//char a[N], b[N], c[N];

	__m128i *av, *bv, *cv;

	printf("working on %d sized data\n",sizeof(int));

	av = (__m128i*)a; // assume 8-byte aligned
	bv = (__m128i*)b; // assume 8-byte aligned
	cv = (__m128i*)c; // assume 8-byte aligned

	for(i=0; i< N; i++){
		a[i] = 0;
		b[i] = 0;
		c[i] = 1;
	}

	if(argv[1] && strcmp(argv[1],"scaling")==0){
	for(j = 0 ; j<CYCLES; j++)
		for (i = 0; i < N/4; i++)
		av[i] = _mm_add_epi32(bv[i], cv[i]);
	}
	else{

	for(j = 0 ; j<CYCLES; j++)
		for (i = 0; i < N; i++)
  			a[i] = b[i] + c[i];
	}

	for(i=0; i< N; i++)
		printf("c[%d] = %d - ",i,c[i]);

}
