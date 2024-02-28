
/*********************************************************/
/*************** client process **************************/
/*********************************************************/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "header.h"


int my_id_coda,id_coda, ret, STATUS;
long key;

request_msg  request_message;
response_msg response_message;


int main(int argc, char *argv[]) {

    key = getpid();

    my_id_coda = msgget(key, IPC_CREAT|IPC_EXCL|0666);
    if( my_id_coda == -1){
	my_id_coda = msgget(key, IPC_CREAT|0666);
	ret = msgctl(my_id_coda,IPC_RMID,0);
	my_id_coda = msgget(key, IPC_CREAT|IPC_EXCL|0666);
	if( my_id_coda == -1 ){
	    printf("process %d - cannot install client queue, please check with the problem\n",getpid());
	    exit(EXIT_FAILURE);
	}
    }


    id_coda = msgget(50, IPC_CREAT|0666);
    if( id_coda == -1 ){
	printf("process %d - cannot open server queue, please check with the problem\n",getpid());
	exit(EXIT_FAILURE);
    }

    request_message.mtype = 1;
    request_message.req.service_code = 1;
    request_message.req.response_channel = my_id_coda;
    printf("process %d - response channel has id %d\n",getpid(),my_id_coda);

    if ( msgsnd(id_coda, &request_message, sizeof(request), FLAG) == -1 ) {
	printf("process %d - cannot send request to the server\n",getpid());
	exit(EXIT_FAILURE);
    }
 

    if ( msgrcv(my_id_coda, &response_message, SIZE, 1, FLAG) == -1) { 
	printf("error while receiving the server response, please check with the problem\n");
	exit(EXIT_FAILURE);
    }
    else {
	printf("process %d - server response: %s\n",getpid(),response_message.mtext);
    }

    msgctl(my_id_coda,IPC_RMID,NULL);

    return 0;

}/* end main*/





