#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


#define TAGLIA 128

#define REGULAR_MESSAGE 1
#define SYNCH_MESSAGE 	2

typedef struct{
        long mtype;
        char mtext[TAGLIA];
    } msg;

int ris;

void produttore(int ds_coda) {/* lettura delle stringhe e spedizione sulla coda */ 
        msg messaggio;
        printf("process %d - digitare le stringhe da trasferire (quit per terminare):\n",getpid());
	fflush(stdout);
        do {
            scanf("%s",messaggio.mtext);
            messaggio.mtype = REGULAR_MESSAGE;
            ris = msgsnd(ds_coda, &messaggio, TAGLIA, IPC_NOWAIT);
            if ( ris == -1 ){
		    printf("process %d - errore nella chiamata msgsnd\n",getpid());
		    fflush(stdout);
	    }
        } while( (strcmp(messaggio.mtext,"quit") != 0));

#ifdef SYNCHRONIZED
        messaggio.mtype = SYNCH_MESSAGE;
        ris = msgsnd(ds_coda, &messaggio, 0 , IPC_NOWAIT);
#endif
 	exit(0);
 }

void consumatore(int ds_coda) { /* ricezione delle stringhe e visualizzazione sullo standard output */
        msg messaggio;

#ifdef SYNCHRONIZED
        ris = msgrcv(ds_coda, &messaggio, 0, SYNCH_MESSAGE, 0);
        if ( ris == -1 ){
	    printf("process %d - errore nella chiamata msgrcv\n",getpid());
	    fflush(stdout);
        }
#endif

        do {
            ris = msgrcv(ds_coda, &messaggio, TAGLIA, REGULAR_MESSAGE, 0);
            if ( ris == -1 ){
		    printf("process %d - errore nella chiamata msgrcv\n",getpid());
		    fflush(stdout);
	    }
            printf("process %d - %s\n", getpid(),messaggio.mtext);
	    fflush(stdout);
        } while( (strcmp(messaggio.mtext,"quit") != 0));
        exit(0);
}

int main(int argc, char *argv[]) {
        int  des_coda, status;   long chiave = 40;

        des_coda = msgget(chiave, IPC_CREAT|IPC_EXCL|0666);
        if ( des_coda == -1 ){
	       	printf("process %d - errore nella chiamata msgget\n",getpid());
		fflush(stdout);
		exit(EXIT_FAILURE);
	}
        if ( fork()!=0 ) {
            if ( fork()!=0 ) {
                wait(&status);
                wait(&status);
            }
            else produttore(des_coda);
        }
        else consumatore(des_coda);

        ris = msgctl(des_coda, IPC_RMID, NULL);

        if ( ris == -1 ){
	       	printf("process %d - errore nella chiamata msgctl",getpid());
		fflush(stdout);
	}

	return 0;

}

