/*********************************************************/
/*************** client process **************************/
/*********************************************************/

#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define SIZE  128
#define MAX_NAME_SIZE  128

#define REQUEST_CODE 1


typedef struct {
	char queue_name[MAX_NAME_SIZE];
	int service_code;
} request;


typedef struct {
	request req;
} request_msg;


typedef struct {
	char mtext[SIZE];
} response_msg;


HANDLE my_slot_id, server_slot_id;
long key;


char queue_name[MAX_NAME_SIZE];
request_msg  request_message;
response_msg response_message;


int main(int argc, char *argv[]) {

	DWORD writtenchars, readchars;
	int aux;

	key = GetCurrentProcessId();

	sprintf(queue_name, "\\\\.\\mailslot\\client%d", key);
	my_slot_id = CreateMailslot(
		queue_name,
		sizeof(response_msg),
		MAILSLOT_WAIT_FOREVER,
		NULL);


	if (my_slot_id == INVALID_HANDLE_VALUE) {
		printf("cannot install client mail-slot with name %s, please check with the problem\n",queue_name);
		fflush(stdout);
		ExitProcess(1);
	}


	server_slot_id = CreateFile(
		"\\\\.\\mailslot\\server",
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,  // required to write to a mailslot
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (server_slot_id == INVALID_HANDLE_VALUE) {
		printf("cannot open server mail-slot, please check with the problem\n");
		scanf("%d", &aux);
		ExitProcess(1);
	}

	request_message.req.service_code = REQUEST_CODE;
	strcpy(request_message.req.queue_name, queue_name);

	if (WriteFile(server_slot_id, &request_message,
		sizeof(request), &writtenchars, NULL) == 0) {
		printf("cannot send request to the server\n");
		fflush(stdout);
		ExitProcess(1);
	}


	if (ReadFile(my_slot_id, &response_message, SIZE, &readchars, NULL) == 0) {
		printf("error while receiving the server response, please check with the problem\n");
		fflush(stdout);
		ExitProcess(1);
	}
	else {
		printf("server response: %s\n", response_message.mtext);
		fflush(stdout);
		ExitProcess(0);
	}

}/* end main*/
