
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define Errore_(x) { puts(x); exit(1); }

#define DATA_SIZE 30

int main(int argc, char *argv[]) {

        char messaggio[DATA_SIZE];
        int  pid, status, fd[2];
	int ret;

        ret = pipe(fd); /* crea una PIPE */
        if ( ret == -1 ) Errore_("Errore nella chiamata pipe");

        pid = fork();   /* crea un processo figlio */
        if ( pid == -1 ) Errore_("Errore nella fork");


        if ( pid == 0 ) {    /* processo figlio: lettore */
               close(fd[1]); /* il lettore chiude fd[1] */
               while( read(fd[0], messaggio, DATA_SIZE) > 0 )
                           printf("process %d - letto messaggio: %s", getpid(),messaggio);
               close(fd[0]);
        }

	/* processo padre: scrittore */
        else {

            close(fd[0]);

            printf("processo %d - digitare testo da trasferire (quit per terminare):\n",getpid());

            do {
                fgets(messaggio,DATA_SIZE,stdin);
                write(fd[1], messaggio, DATA_SIZE);
                printf("processo %d - scritto messaggio: %s", getpid(), messaggio);
            } while( strcmp(messaggio,"quit\n") != 0 );

            close(fd[1]);

            wait(&status);
        }

    }

