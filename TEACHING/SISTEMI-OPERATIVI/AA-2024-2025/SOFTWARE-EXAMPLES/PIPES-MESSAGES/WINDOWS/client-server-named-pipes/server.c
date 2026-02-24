#include <stdio.h>
#include <windows.h>

#define SIZE 128

typedef struct {
	long code;
	char request_string[SIZE];
} request;



int main(int argc, char *argv[]) {

	DWORD tid;
	DWORD err;
	request r;
	char  *response = (char*)"done";
	DWORD result;
	HANDLE hThreadHandle;
	HANDLE my_pipe;


	while (1) {

		my_pipe = CreateNamedPipe("\\\\.\\pipe\\serv",
			PIPE_ACCESS_DUPLEX, 0, PIPE_UNLIMITED_INSTANCES,
			1024, 1024, 0, NULL);


		if (my_pipe == INVALID_HANDLE_VALUE) {
			printf("Errore nella chiamata CreateNamedPipe\n");
			fflush(stdout);
			ExitProcess(1);
		}


		if (!ConnectNamedPipe(my_pipe, NULL)) {
			err = GetLastError();
			if (err != ERROR_PIPE_CONNECTED) {
				printf("Errore nella chiamata ConnectNamedPipe\n");
				fflush(stdout);
				ExitProcess(1);
			}
		}

		if (!ReadFile(my_pipe, &r, sizeof(r), &result, NULL)) {
			printf("Errore nella chiamata readfile\n");
			ExitProcess(1);
		}

		printf("service request at process %d - (code is %d - payload string is = %s)\n", GetCurrentProcessId(), r.code, r.request_string);
		fflush(stdout);

		if (!WriteFile(my_pipe, response, strlen(response)+1, &result, NULL)) {
			printf("Errore nella chiamata WriteFile\n");
			ExitProcess(1);
		}


		CloseHandle(my_pipe);//alternatively you can stil use the same named pipe
							//after using DisconnectNambedPipe
	}

	return(0);
}

