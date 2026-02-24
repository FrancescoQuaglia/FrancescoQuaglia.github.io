// environment audit on parent or child (if any)
// please compile with ASCII settings

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>



int main(int argc, char *argv[]) {
	BOOL newprocess;
	STARTUPINFO si;
	PROCESS_INFORMATION  pi;
	int i = 0;
	char* p = (char*)&argc;
	char pwd[MAX_PATH];
	LPVOID lpvEnv;
	LPTSTR lpszVariable;


	GetCurrentDirectory(MAX_PATH, pwd);
	printf("process is %d - current directory is: %s\n\n", GetCurrentProcessId(), pwd);

	/* print argv elements */
	while (i < argc){
		printf("arg %d is: %s\n", i, argv[i]);
		i++;
	}

	if (argc>1){
		memset(&si, 0, sizeof(si));
		memset(&pi, 0, sizeof(pi));
		si.cb = sizeof(si);

		printf("trying running the child command: %s\n\n", argv[1]);
		fflush(stdout);
		

		newprocess = CreateProcess(argv[1],
			"",
			NULL,
			NULL,
			FALSE,
			NORMAL_PRIORITY_CLASS,
			"A=10\0B=20\0\0",
			//		NULL,
			//"MY_VARIABLE=34\0\0", // please try the various alternatives here
			NULL,
			&si,
			&pi);
			

		if (newprocess == FALSE) {
			printf("CreateProcess failed - no child command has been run\n");
			ExitProcess(-1);
		}


		WaitForSingleObject(pi.hProcess, INFINITE);

		ExitProcess(0);
	}
	else{


		lpvEnv = GetEnvironmentStrings();

		if (lpvEnv == NULL)
			printf("GetEnvironmentStrings() failed.\n");
		else
			printf("GetEnvironmentStrings() is OK.\n\n");

		for (lpszVariable = (LPTSTR)lpvEnv; *lpszVariable;)	{

			printf("%s\n", lpszVariable);
			lpszVariable += strlen(lpszVariable) + 1;

		}
	}

}





