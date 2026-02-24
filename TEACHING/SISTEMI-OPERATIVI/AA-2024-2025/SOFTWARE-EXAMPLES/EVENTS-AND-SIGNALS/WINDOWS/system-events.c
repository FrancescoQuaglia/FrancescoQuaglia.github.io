// this program traps the CTRL+C system event


#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buff[1024];

int one_shot = 1;

int  myhandler(int event){

	printf("received CTRL+C\n");
	fflush(stdout);
	while (1) {
		printf("%s\n",buff);
		if (one_shot) break;
		Sleep(1000);
	};
	return  1;
}


int _tmain(int argc, _TCHAR* argv[])
{
	BOOL result;
	result = SetConsoleCtrlHandler((PHANDLER_ROUTINE)myhandler, 1);
	printf("handler posting returned %d \n", result);
	fflush(stdout);
	while (1){
		scanf("%s", buff);
	//	break;
	}
	return 0;
}


