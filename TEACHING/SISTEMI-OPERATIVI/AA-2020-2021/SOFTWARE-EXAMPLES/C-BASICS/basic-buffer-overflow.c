#include <stdio.h>

int control_variable;
int v[10];

int main(int argc, char * argv[]){

	int index, value;

	int *k;

	k = &control_variable;

	*k;

#ifdef IN_STACK
	int control_variable;
	int v[10];
#endif

	control_variable = 1;

	while (control_variable == 1){
		scanf("%d%d",&index,&value);
		v[index] = value;
		printf("array elem at position %d has been set to value %d\n",index,v[index]);


	}

	printf("exited cycle\n");

	return 0;
}

