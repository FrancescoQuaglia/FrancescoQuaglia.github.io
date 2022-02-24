// processes-priorities.cpp : Defines the entry point for the console application.
//



#include "stdafx.h"
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
	char prio[128];
	int ret;

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
			"child",
			NULL,
			NULL,
			FALSE,
			NORMAL_PRIORITY_CLASS,
			NULL,
			NULL,
			&si,
			&pi);

		if (newprocess == FALSE) {
			printf("CreateProcess failed - no child command has been run\n");
			ExitProcess(-1);
		}

		printf("created new process\n");

		while (1){
			printf("please give me the new priority to set for child process\n");
			fflush(stdout);
			scanf("%s", prio);
			if (strcmp(prio, "none") == 0) break;
			if (strcmp(prio, "high") == 0){
				ret = SetPriorityClass(pi.hProcess, HIGH_PRIORITY_CLASS);
			}
			else{
				ret = SetPriorityClass(pi.hProcess, NORMAL_PRIORITY_CLASS);
			}
			printf("set priority returned %d\n", ret);
		}

		TerminateProcess(pi.hProcess, 0);
		WaitForSingleObject(pi.hProcess, INFINITE);

		ExitProcess(0);
	}
	else{

		while (1);

		
	}

}





