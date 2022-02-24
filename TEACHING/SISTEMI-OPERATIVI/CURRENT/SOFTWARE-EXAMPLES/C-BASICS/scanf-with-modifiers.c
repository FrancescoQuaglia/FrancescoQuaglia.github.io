#include<stdio.h>
#include<stdlib.h>


int main(int argc, char**argv){

	char *p = NULL;

	scanf("%ms",&p);
	printf("%s\n",p);
	free(p);
	p = NULL;

}
