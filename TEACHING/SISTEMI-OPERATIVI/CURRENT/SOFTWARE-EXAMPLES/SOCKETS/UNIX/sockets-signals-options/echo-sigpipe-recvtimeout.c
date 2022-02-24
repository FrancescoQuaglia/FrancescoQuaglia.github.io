#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define BACKLOG 1
#define SIZE 128
#define ECHO_LENGTH 2


char buff[SIZE];

int comm_sock;

void generic_handler(int signal, siginfo_t *info, void* unused){

        printf("received signal is %d\n",signal);
	close(comm_sock);
}


int main() {

	int ds_sock;
	struct sockaddr_in my_addr, their_addr;
	int ret;
	unsigned short port = htons(25000);
	int size;
	sigset_t set;
  	struct sigaction act;
	char* IP;
	struct timeval timeout;
	int i;
	int on = 1;

	ds_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (ds_sock == -1){
		printf("socket error\n");
		return -1;
	}
	printf("socket installed - descriptor is %d\n",ds_sock);

	setsockopt(ds_sock,SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	my_addr.sin_family      = AF_INET;
	my_addr.sin_port        = port;
	my_addr.sin_addr.s_addr = INADDR_ANY; 

	ret = bind(ds_sock,(struct sockaddr*) &my_addr, sizeof(my_addr));
	if(ret == -1){
		printf("bind error\n");
		return -1;
	}
	printf("socket address set up ok\n");

	ret = listen(ds_sock, BACKLOG);
	if(ret == -1){
		printf("listen error\n");
		return -1;
	}
	printf("socket listen set up ok\n");

	sigemptyset(&set);
	act.sa_sigaction = generic_handler; 
	act.sa_mask =  set;
  	act.sa_flags = SA_SIGINFO;
  	act.sa_restorer = NULL;
  	sigaction(SIGPIPE,&act,NULL);



	new_accept:
	comm_sock =  accept(ds_sock,(struct sockaddr*)&their_addr,&size);
	if(comm_sock == -1){
		printf("accept error\n");
		return -1;
	}

#ifdef SOURCE_INFO
	IP = (char*)inet_ntoa(their_addr.sin_addr);
	printf("comm sock is %d - connection source is %s, port number is %u, start logging\n",comm_sock,(char*)IP,their_addr.sin_port);
#endif

#ifdef RECEIVE_TIMEOUT
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	setsockopt(comm_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#endif

#ifdef ALARM
	sigemptyset(&set);
	act.sa_sigaction = generic_handler; 
	act.sa_mask =  set;
  	act.sa_flags = SA_SIGINFO;
  	act.sa_restorer = NULL;
  	sigaction(SIGALRM,&act,NULL);
#endif

	while(1){
#ifdef ALARM
		alarm(5);
#endif
#ifndef PADDING
		ret = read(comm_sock,buff,SIZE);
#else
		ret = recv(comm_sock,buff,SIZE,MSG_WAITALL);
#endif
#ifdef ALARM
		alarm(0);
#endif

		if(ret<=0) break;
		printf("echoing this: ");
		fflush(stdout);
		write(1,buff,ret);
		for (i=0; i< ECHO_LENGTH; i++){
			sleep(1);
#ifndef HANDLE_SIGPIPE
			ret = write(comm_sock,buff,ret);
#else
			ret = send(comm_sock,buff,ret,MSG_NOSIGNAL);
#endif
		}
	}
	close(comm_sock);
	printf("connection closed - accepting a new one\n");
	fflush(stdout);
	goto new_accept;
	return 0;
}
