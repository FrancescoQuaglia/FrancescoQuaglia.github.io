#include <stdio.h>

int main (int a, char** b){

	long value;
	long expected;
	scanf("%ld",&value);
	printf("starting from value %ld\n",value);
	while(1){
		expected = value +1;
		if (scanf("%ld",&value)==EOF) return 0;
		
		printf("%ld\n",value);
		if(value != expected){
			printf("sequence broken at expected value %ld!!\n",expected);
			return 0;
		}

	}
}
