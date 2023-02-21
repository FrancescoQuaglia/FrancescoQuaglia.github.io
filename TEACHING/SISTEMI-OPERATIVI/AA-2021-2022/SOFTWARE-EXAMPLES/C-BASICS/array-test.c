//this test shows how to use type casting, particularly with pointers
//it also shows how to use negative pointer displacements for accessing memory

#include <stdio.h>

int a[10] = {1,2,3,4,5,6,7,8,9,10};

int main(int argc, char**argv){

	char *p;
	int i;

	for(i=0; i<10; i++){
		printf("%d ",a[i]);

	}
	printf("\n");

	//p = (char*)a + sizeof(a);//setting p to the upper boundary address of the array a[]
	p = a + sizeof(a);//is this setting p to the upper boundary address of the array a[]??

	for(i=1; i<=10; i++){
		printf("%d ",((int*)p)[-i]); //here we use negative pointer displacement for accessing the array of integers a[]

	}
	printf("\n");
}

