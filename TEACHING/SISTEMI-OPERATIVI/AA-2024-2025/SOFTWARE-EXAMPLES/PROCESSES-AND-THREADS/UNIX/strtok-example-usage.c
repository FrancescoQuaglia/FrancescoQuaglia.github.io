#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_TOKENS 128

int gets(char*);

char line[4096];

char* s = " ";//blank is used as the tokenizer character

#ifdef SEGFAULT_TEST
char * pointer = "ciao a tutti";
#endif

int main(int argc, char** argv) {
	char **token_vector;
      	char* p;
      	char* tokens[MAX_TOKENS];
      	int i;

#ifdef SEGFAULT_TEST
	p = (char*)strtok(pointer,s);//you should never try to do this
#endif

	gets(line);
	p = (char*)strtok(line,s);//tokenize and get the pointer to the first token


	i = 0;
	tokens[i] = p;

	while(p){//get the pointers to the other tokens
	  	p = (char*)strtok(NULL,s);
		tokens[++i] = p;
		
	} 
	tokens[++i] = NULL;

	token_vector = tokens;

	while(*token_vector){//audit the tokens on the standard output
		printf("%s\n",*token_vector);
		token_vector++;

	}
}
