#include<stdio.h>

int x;
double *y;

void main(void){
	y = &x;
 	printf("x address is %p - y is %p | x address+1 is %p - y+1 is %p\n",&x,y,&x+1,y+1);
}
