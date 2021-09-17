
//#define _CTR_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>   //actual stuff inclided will depend on macros such as _CRT_SECURE_NO_WARNINGS - you can set it manually or via IDE
#include <stdlib.h>



#define ECHOING_EXIT_CODE 2
#define EXIT_CODE 33

int main(int argc, char *argv[]) {

	BOOL newprocess;
	STARTUPINFO si;
	PROCESS_INFORMATION  pi;
	DWORD exitcode;
	char buff[128];

	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(si);


	if (argc == 2){
		printf("Starting up echoing process ....\n");
		fflush(stdout);
		newprocess = CreateProcessA((LPCSTR)argv[1], (LPSTR)argv[1], NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, (LPSTARTUPINFO)&si, &pi);
		if (newprocess == FALSE) {
			printf(" failure\n\n");
			ExitProcess(1);
		};
		printf(" done\n\n");

		Sleep(5000);
		TerminateProcess(pi.hProcess, EXIT_CODE);
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &exitcode);
		printf("\nEchoing process exited with code %d\n", exitcode);
		fflush(stdout);

	}
	else{
		printf("\tHere I'm for echoing\n\t");
		fflush(stdout);
		while (1) {
			scanf("%s", buff);
			if (strcmp(buff, "bye") == 0) ExitProcess(ECHOING_EXIT_CODE);
			printf("\t%s\n\t", buff);
			fflush(stdout);
		}
	}
}

