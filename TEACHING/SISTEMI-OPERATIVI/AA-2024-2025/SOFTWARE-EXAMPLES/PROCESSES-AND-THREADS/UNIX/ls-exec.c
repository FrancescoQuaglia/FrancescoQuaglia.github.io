#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	printf("programma attivato\n");
        execlp("ls","ls",NULL);
        printf("La chiamata execlp() ha fallito\n");
}
