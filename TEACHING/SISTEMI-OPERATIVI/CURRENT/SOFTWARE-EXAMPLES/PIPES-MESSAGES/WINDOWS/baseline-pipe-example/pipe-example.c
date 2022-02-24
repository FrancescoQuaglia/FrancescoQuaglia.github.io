#include <stdio.h>
#include <windows.h>

#define Errore_(x) { puts(x); ExitProcess(1); }

int main(int argc, char *argv[]) {

	char messaggio[30];
	SECURITY_ATTRIBUTES security;
	BOOL rit;
	HANDLE readHandle, writeHandle, temp_readHandle;
	DWORD result;
	BOOL newprocess;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	if (argc == 1) { /* Creazione pipe nel parent */
		security.nLength = sizeof(security);
		security.lpSecurityDescriptor = NULL;
		security.bInheritHandle = TRUE;
		rit = CreatePipe(&readHandle, &writeHandle, &security, 0);
		if (!rit) Errore_("Errore nella CreatePipe");


		/* genero il processo scrittore */
		memset(&si, 0, sizeof(si));
		memset(&pi, 0, sizeof(pi));
		si.cb = sizeof(si);

		temp_readHandle = GetStdHandle(STD_INPUT_HANDLE);
		SetStdHandle(STD_INPUT_HANDLE, readHandle);

		newprocess = CreateProcess((LPCSTR)".\\pipe-example.exe", (LPSTR)".\\pipe-example.exe lettore", NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);        
		
		if (newprocess == 0) { 
			printf("Errore nella generazione del lettore\n");               
			ExitProcess(-1); 
		}          
		
		SetStdHandle(STD_INPUT_HANDLE, temp_readHandle);          
		CloseHandle(readHandle);          
		
		printf("digitare testo da trasferire (quit per terminare):\n");
		fflush(stdout);

		do {
			fgets(messaggio, 30, stdin);
			rit = WriteFile(writeHandle, messaggio, 30, &result, NULL);
			if (!rit) Errore_("Errore nella writefile!");               
			printf("scritto messaggio: %s", messaggio);
			fflush(stdout);
		} while (strcmp(messaggio, "quit\n") != 0);

		CloseHandle(writeHandle);          
		WaitForSingleObject(pi.hProcess, INFINITE);
	}
	else {
		if (argc == 2 && strcmp(argv[1], "lettore") == 0) {
			do {
				rit = ReadFile(GetStdHandle(STD_INPUT_HANDLE), messaggio, 30, &result, NULL);
				if (rit) {
					printf("letto messaggio: %s", messaggio);
					fflush(stdout);
				}
				if (strcmp(messaggio, "quit\n") == 0) break;
			} while (rit);
		}
	}
}
