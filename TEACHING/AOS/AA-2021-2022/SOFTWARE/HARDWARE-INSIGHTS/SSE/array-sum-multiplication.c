#include <stdio.h>

#define N 10000
#define NTIMES 100000

/*
double a[N] __attribute__ ((aligned (16)));
double b[N] __attribute__ ((aligned (16)));
double c[N] __attribute__ ((aligned (16)));
double r[N] __attribute__ ((aligned (16)));
*/
int a[N] __attribute__ ((aligned (16)));
int b[N] __attribute__ ((aligned (16)));
int c[N] __attribute__ ((aligned (16)));
int r[N] __attribute__ ((aligned (16)));

int main (void){

  int i, times;
  int count = 0;

  printf("Managing arrays of %d elements with size %d\n",N,sizeof(double));

  for (times = 0; times < NTIMES; times++)
    {
      for (i = 0; i < N; ++i)
        r[i] = (a[i] + b[i]) * c[i];
	count++;
    }

  printf("count is %d\n",count);

  return 0;
}
