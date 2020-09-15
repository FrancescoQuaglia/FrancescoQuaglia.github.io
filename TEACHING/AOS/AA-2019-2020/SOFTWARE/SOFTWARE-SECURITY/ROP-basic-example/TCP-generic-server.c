

#include <stdio.h>

#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */


#include <stdlib.h>
#include <string.h>
#include <signal.h>


#define MAX_LINE           1024
#define LISTENQ            100 

char buff[MAX_LINE];

#define separation_characters "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" //a generic 16-byte line

char * scattered_code_pieces = {
	"\x48\x31\xc0"                               // xor    %rax,%rax
        "\x99"                                       // cltd
        "\xb0\x3b"                          	// mov    $0x3b,%al
	"\xc3"					//retq
	"\x90"					//nop
	"\x90"					//nop
	"\x90"					//nop
	"\x90"					//nop
	"\x90"					//nop
	"\x90"					//nop
	"\x90"					//nop
	"\x90"					//nop
	"\x90"					//nop
	separation_characters			//48 bytes of other stuff
	separation_characters
	separation_characters
	"\x48\xbf\x2f\x2f\x62\x69\x6e\x2f\x73\x68"   // mov $0x68732f6e69622fff,%rdi
        "\x48\xc1\xef\x08"                           // shr    $0x8,%rdi
	"\xc3"					//retq
	"\x90"					//nop
	separation_characters			//48 bytes of other stuff
	separation_characters
	separation_characters
        "\x57"                                       // push   %rdi
	"\x48\x89\xe7"                               // mov    %rsp,%rdi
        "\x57"                                       // push   %rdi
        "\x52"                                       // push   %rdx
        "\x48\x89\xe6"                               // mov    %rsp,%rsi
        "\x0f\x05"                                  // syscall
};

void show(char*str, int size){

 /* this function will be smashed by any stream long 144 bytes or more
  * except if you use canary or protect the stack from being executable
  * with streams shorter than 137 bytes it will work correctly */

 unsigned long buffer[16];

 memcpy((char*)buffer,(char*)str,size);

 printf("I'm helping you, the code patch work is at address \n%p",scattered_code_pieces);

 printf("%s\n",(char*)buffer);
 return;

}


int main(int argc, char**argv) {
  int x,i,ret,fd;


    int       list_s;                /*  listening socket          */
    int       conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    struct    sockaddr_in their_addr;
    char      buffer[MAX_LINE];      /*  character buffer          */
    int sin_size;
    int option = 1;
   
    if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		fprintf(stderr, "server: socket creation error.\n");
		exit(EXIT_FAILURE);
    }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(2345);
	
    if (setsockopt(list_s, SOL_SOCKET, SO_REUSEADDR, (void*)&option, sizeof(int)) < 0){
        fprintf(stderr, "setsockopt(SO_REUSEADDR) failed.\n");
	exit(EXIT_FAILURE);
    }

    /*  Bind our socket addresss to the 
	listening socket, and call listen()  */

    if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
		fprintf(stderr, "server: socket bind error.\n");
		exit(EXIT_FAILURE);
    }

    if ( listen(list_s, LISTENQ) < 0 ) {
		fprintf(stderr, "server: socket listen error.\n");
		exit(EXIT_FAILURE);
    }

    
    /*  Enter an infinite loop to respond
        to client requests and echo input  */
    
    signal(SIGCHLD, SIG_IGN);

    while ( 1 ) {

		/*  Wait for a connection, then accept() it  */
		if ( (conn_s = accept(list_s, (struct sockaddr *)&their_addr, &sin_size) ) < 0 )
		{
		    fprintf(stderr, "server: accept error.\n");
	    	    exit(EXIT_FAILURE);
		}

		printf("server: incoming connection from %s\n", inet_ntoa(their_addr.sin_addr));
		/*  Retrieve an input line from the connected socket
		    then simply write it back to the same socket.     */
		if (fork()){

			//parent - do what you want
		}
		else{

		  close(0);
		  dup2(conn_s,0);
		  close(1);
		  dup2(conn_s,1);

   		  ret = 1;
		  while(ret>0) {

		   ret=read(0,buff,1024);			
		   if(ret <= 0) exit(-1);
  		   buff[ret]='\0';
		   show(buff,ret); //I'm echoing on terminal
		 }

		}//end else
	}

}

