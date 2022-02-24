#include <windows.h>
#include <stdio.h>
#include <stdlib.h>


DWORD ThreadFiglio();


int main(int argc, char *argv[]) {

	HANDLE hThread;
	DWORD hid;
	DWORD exit_code;
	
	hThread = CreateThread(NULL,
		0,
		(LPTHREAD_START_ROUTINE)ThreadFiglio,
		NULL,
		NORMAL_PRIORITY_CLASS,
		&hid);



	if (hThread == NULL)  printf("Thread creation failure\n");
	else {
		WaitForSingleObject(hThread, INFINITE);
		GetExitCodeThread(hThread, &exit_code);
		printf("child thread exited with code %d\n", exit_code);
		fflush(stdout);
	}
	printf("parent thread - give me an integer to make me exit:");
	fflush(stdout);

	scanf("%d", &exit_code);
}



DWORD ThreadFiglio()
{

	int x;
	printf("child thread - give me an integer to make me exit:");
	fflush(stdout);
	scanf("%d", &x);
	ExitThread(x);
}

