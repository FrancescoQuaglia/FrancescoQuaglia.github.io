
#include <windows.h>
#include <stdio.h>

#define EXPOSE

#ifdef EXPOSE
int * v[2];
#endif

__declspec(thread) int  variable;

void ChildThread(char * me)
{

	printf("Child thread %d active - variable is %d\n", (int)me,variable);
	fflush(stdout);
	
#ifdef EXPOSE
	variable = (int)me + 33;
	v[(int)me] = &variable;
#endif
	Sleep(3000);
	ExitThread((DWORD)me);
}

int main(int argc, char** argv){
	HANDLE hThread[2];
	int i;
	DWORD hid;

	variable = 1;

	for (i = 0; i < 2; i++){

		hThread[i] = CreateThread(NULL,
			0,
			(LPTHREAD_START_ROUTINE)ChildThread,
			(LPVOID)i,
			NORMAL_PRIORITY_CLASS,
			&hid);

		if (hThread[i] == NULL) {
			printf("cannot create thread %d\n", i);
			ExitProcess(-1);
		}

	}

#ifdef EXPOSE
	Sleep(1000);
	for (i = 0; i < 2; i++){
		printf("thread %d - variable value is %d\n", i, *v[i]);
	}
#endif

	for (i = 0; i < 2; i++){
		WaitForSingleObject(hThread[i], INFINITE);
	}

}

