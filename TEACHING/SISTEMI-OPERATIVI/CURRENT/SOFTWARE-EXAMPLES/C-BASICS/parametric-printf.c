#include <stdio.h>

char* text[3] = {"ennio - matricola %d\n",
		  "luigi - matricola %d\n",
		  "mario - matricola %d\n"};

int matricola[3] = {0, 1, 2};
 
int main(int a, char** b){

	int i;

	for(i=0; i < 3 ;i++)
		printf(text[i],matricola[i]);
}
