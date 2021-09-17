
/******************************************************************
this program allows sorting strings coming form the standar input
it uses one thread for input and one thread at a time for sorting 
*******************************************************************/


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR	1000
#define MAX_STRING      1000


int string_number = 0;
char *string_array[MAX_STRING];
DWORD exit_code;

void OrderThread(char *new_string)
{
	int i, j;

	//printf("Ordering thread active (inserting: %s)\n",new_string);

	for (i = 0; i < string_number; i++) {
		if (strcmp(new_string, string_array[i]) <= 0) {
			for (j = 0; j<(string_number - i); j++) {
				string_array[string_number - j] = string_array[string_number - j - 1];
			}
			break;
		}
	}
	string_array[i] = new_string;
	string_number++;
	exit_code = 0;
	ExitThread(exit_code);
}

int main(int argc, char** argv){
	HANDLE hThread;
	DWORD hid;
	int i;
	char *old_buffer;
	DWORD exit_code;
	char buffer[MAX_CHAR];
	int notfirst = 0;

	while (1) {
		printf("Inserisci stringa: ");
		fflush(stdout);
		scanf("%s", buffer);
		if (strcmp(buffer, "quit") == 0) break;
		old_buffer = strdup(buffer);
		if (notfirst) {
			WaitForSingleObject(hThread, INFINITE);
			GetExitCodeThread(hThread, &exit_code);
		}
		else notfirst = 1;



		hThread = CreateThread(NULL,
			0,
			(LPTHREAD_START_ROUTINE)OrderThread,
			(LPVOID)old_buffer,
			NORMAL_PRIORITY_CLASS,
			&hid);

		if (hThread == NULL) {
			printf("cannot create thread %d\n");
			ExitProcess(-1);
		}
	}
	if (notfirst) {
		WaitForSingleObject(hThread, INFINITE);
		GetExitCodeThread(hThread, &exit_code);
	}
	for (i = 0; i< string_number; i++) printf("String %d: %s\n", i, string_array[i]);
	ExitProcess(0);
}


