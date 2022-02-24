/* 

	UDPSERVER.C
	===========
	
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close() */
#include <string.h> /* memset() */
#include <stdlib.h>

#define LOCAL_SERVER_PORT 7000
#define MAX_MSG 100

int main(int argc, char *argv[])
{
	int sd, rc, n, cliLen;
	struct sockaddr_in cliAddr, servAddr;
  	char msg[MAX_MSG];

  	/* socket creation */
  	sd=socket(AF_INET, SOCK_DGRAM, 0);
  	if(sd<0) 
  	{
    	printf("%s: errore nell'apertura dell socket.\n",argv[0]);
    	exit(1);
  	}

  	/* bind local server port */
  	servAddr.sin_family = AF_INET;
  	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  	servAddr.sin_port = htons(LOCAL_SERVER_PORT);
  	rc = bind (sd, (struct sockaddr *) &servAddr,sizeof(servAddr));
  	if(rc<0)
	{
    	printf("%s: errore nella bind %d \n", 
			argv[0], LOCAL_SERVER_PORT);
    	exit(1);
  	}

  	printf("%s: in attesa di dati sulla porta UDP %u\n", 
		argv[0],LOCAL_SERVER_PORT);

  	/* server infinite loop */
  	while(1)
	{
		/* init buffer */
    	memset(msg,0x0,MAX_MSG);


	    /* receive message */
    	cliLen = sizeof(cliAddr);
	    n = recvfrom(sd, msg, MAX_MSG, 0, 
			(struct sockaddr *) &cliAddr, &cliLen);

    	if (n<0)
		{
      		printf("%s: impossibile ricevere dati \n",argv[0]);
      		continue;
    	}
      
    	/* print received message */
    	printf("%s: dati da %s:UDP%u : %s \n", 
	    	argv[0],inet_ntoa(cliAddr.sin_addr),
				ntohs(cliAddr.sin_port),msg);
    
  	}/* end of server infinite loop */

	return 0;

}
