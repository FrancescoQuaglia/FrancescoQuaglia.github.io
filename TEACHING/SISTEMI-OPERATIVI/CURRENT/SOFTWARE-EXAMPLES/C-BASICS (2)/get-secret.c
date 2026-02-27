#include <stdio.h>

char format_line[4096];

char *secret = "francesco";

unsigned long x = 0x402004; 

int main(int a, char** b){

	printf("secret is at %p\n",secret);

	while(1){
		scanf("%s",format_line);
		printf(format_line,x);
		printf("\n");

	}
}
