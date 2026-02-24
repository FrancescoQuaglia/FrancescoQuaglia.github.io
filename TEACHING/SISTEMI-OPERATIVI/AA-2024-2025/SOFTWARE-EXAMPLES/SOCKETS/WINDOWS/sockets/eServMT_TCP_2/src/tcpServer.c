/*

  TCPSERVER.C
  ==========

*/



#include <stdio.h>
#include <winsock.h>
#include <stdlib.h>

#include "helper.h"           /*  Our own helper functions  */

/*  Global constants  */

#define MAX_LINE           (1000)

int ParseCmdLine(int argc, char *argv[], char **szPort);
void receiveThread(void* sd);

int main(int argc, char *argv[])
{
    SOCKET    list_s;                /*  listening socket          */
    SOCKET    conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    struct	  sockaddr_in their_addr;
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *endptr;                /*  for strtol()              */
	int 	  sin_size;   
	DWORD     nThread;
	WSADATA   wsaData;
	
	/*  Get command line arguments  */

    ParseCmdLine(argc, argv, &endptr);
	
	port = strtol(endptr, &endptr, 0);
	if ( *endptr )
	{
	    fprintf(stderr, "server: porta non riconosciuta.\n");
	    exit(EXIT_FAILURE);
	}
    
	printf("Server in ascolto sulla porta %d\n",port);

	
	if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0)
	{
		printf("errore in WSAStartup()\n");
		exit(EXIT_FAILURE);
	}

	/*  Create the listening socket  */

    if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET )
	{
		fprintf(stderr, "server: errore durante la creazione della socket.\n");
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

    if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) == SOCKET_ERROR )
	{
		fprintf(stderr, "server: errore durante la bind.\n");
		exit(EXIT_FAILURE);
    }

    if ( listen(list_s, LISTENQ) == SOCKET_ERROR)
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
		if ( (conn_s = accept(list_s, (struct sockaddr *)&their_addr, &sin_size) ) == INVALID_SOCKET )
		{
		    fprintf(stderr, "server: \n");
	    	exit(EXIT_FAILURE);
		}

		printf("server: connessione da %s:%d\n", inet_ntoa(their_addr.sin_addr),their_addr.sin_port);
		/*  Retrieve an input line from the connected socket
		    then simply write it back to the same socket.     */
		CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)receiveThread,	/* is equivalent to declare 
													   DWORD WINAPI receiveThread(void*) */
			(LPVOID)conn_s,							/* is equivalent to (void*)conn_s    */
			NORMAL_PRIORITY_CLASS,
			&nThread);	
    }

	if ( closesocket(list_s) == SOCKET_ERROR )
	{
		fprintf(stderr, "server: errore durante la close.\n");
		exit(EXIT_FAILURE);
	}
	WSACleanup();
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

void receiveThread(void* sd)
{
	char buf[MAX_LINE];
	SOCKET conn = (SOCKET)sd;
	
	Readline(conn, buf, MAX_LINE-1);
	printf("server: il client ha scritto\n\t%s\n",buf);
	Writeline(conn, buf, strlen(buf));

	/*  Close the connected socket  */

	closesocket(conn);
}