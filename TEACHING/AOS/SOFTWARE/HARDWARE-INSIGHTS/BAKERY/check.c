#include <stdio.h>

int main (int a, char** b){

	long value;
	long expected;
	scanf("%d",&value);
	printf("starting from value %d\n",value);
	while(1){
		expected = value +1;
		if (scanf("%d",&value)==EOF) return 0;
		
		printf("%d\n",value);
		if(value != expected){
			printf("sequence broken at expected value %d!!\n",expected);
			return 0;
		}

	}
}
