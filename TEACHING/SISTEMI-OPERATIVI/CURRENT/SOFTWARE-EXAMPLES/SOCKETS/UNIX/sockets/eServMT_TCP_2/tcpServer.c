/*

  TCPSERVER.C
  ==========

*/


#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include "helper.h"           /*  our own helper functions  */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <pthread.h>

/*  Global constants  */

#define MAX_LINE           (1000)

int ParseCmdLine(int argc, char *argv[], char **szPort);
void * receiveThread(void * param);

int status;

int main(int argc, char *argv[])
{
    int       list_s;                /*  listening socket          */
    pthread_t tid;
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    struct	  sockaddr_in their_addr;
	char      buffer[MAX_LINE];      /*  character buffer          */
    char     *endptr;                /*  for strtol()              */
	int 	  sin_size;    
	int 	  proc_id;
	void     *status;
	int       conn_s;                /*  connection socket         */
	
	/*  Get command line arguments  */
	ParseCmdLine(argc, argv, &endptr);
	
	port = strtol(endptr, &endptr, 0);
	if ( *endptr )
	{
	    fprintf(stderr, "server: porta non riconosciuta.\n");
	    exit(EXIT_FAILURE);
	}
    
	printf("Server in ascolto sulla porta %d.\n",port);
	
	/*  Create the listening socket  */

    if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		fprintf(stderr, "server: errore nella creazione della socket.\n");
		exit(EXIT_FAILURE);
    }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);


    /*  Bind our socket addresss to the 
	listening socket, and call listen()  */

    if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 )
	{
		fprintf(stderr, "server: errore durante la bind.\n");
		exit(EXIT_FAILURE);
    }

    if ( listen(list_s, LISTENQ) < 0 )
	{
		fprintf(stderr, "server: errore durante la listen.\n");
		exit(EXIT_FAILURE);
    }

    
    /*  Enter an infinite loop to respond
        to client requests and echo input  */

    while ( 1 )
	{

		/*  Wait for a connection, then accept() it  */

		sin_size = sizeof(struct sockaddr_in);
		if ( (conn_s = accept(list_s, (struct sockaddr *)&their_addr, &sin_size) ) < 0 )
		{
		    fprintf(stderr, "server: \n");
	    	exit(EXIT_FAILURE);
		}

		printf("server: connessione da %s\n", inet_ntoa(their_addr.sin_addr));
		/*  Retrieve an input line from the connected socket
		    then simply write it back to the same socket.     */

		proc_id=pthread_create(&tid, NULL, receiveThread, (void*)&conn_s);

		if (proc_id)
		{	
			printf("server: impossibile creare il thread. Errore: %d\n", proc_id);
      		exit(EXIT_FAILURE);
      	}		
    }
	pthread_join(tid, &status);
	
	if ( close(list_s) < 0 )
	{
		fprintf(stderr, "server: errore durante la close.\n");
		exit(EXIT_FAILURE);
	}
}

int ParseCmdLine(int argc, char *argv[], char **szPort)
{
    int n = 1;

    while ( n < argc )
	{
		if ( !strncmp(argv[n], "-p", 2) || !strncmp(argv[n], "-P", 2) )
			*szPort = argv[++n];
		else 
			if ( !strncmp(argv[n], "-h", 2) || !strncmp(argv[n], "-H", 2) )
			{
			    printf("Sintassi:\n\n");
	    		printf("    server -p (porta) [-h]\n\n");
			    exit(EXIT_SUCCESS);
			}
		++n;
    }
	if (argc==1)
	{
	    printf("Sintassi:\n\n");
		printf("    server -p (porta) [-h]\n\n");
	    exit(EXIT_SUCCESS);
	}
    return 0;
}

void * receiveThread(void * param)
{
	char buf[MAX_LINE];
	int my_sock = *((int*)param);
	
	Readline(my_sock, buf, MAX_LINE-1);
	printf("server: il client ha scritto\n\t%s\n", buf);
	Writeline(my_sock, buf, strlen(buf));

	close(my_sock);

	pthread_exit((void *)&status);
}
