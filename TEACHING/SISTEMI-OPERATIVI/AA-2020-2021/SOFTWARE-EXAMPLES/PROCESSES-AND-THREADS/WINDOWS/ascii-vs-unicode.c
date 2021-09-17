// ASCII-vs-UNICODE.c : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>

TCHAR *p = TEXT("francesco");
TCHAR *format = TEXT("%ls\n");


int main(int argc, _TCHAR* argv[])
{
	int size;
	size = _tcslen(p);
	printf("size is %d\n", size);
	printf("size of tchar is %d\n", sizeof(TCHAR));


	wprintf(format, p);
	wprintf(L"%d\n", 33);
	wprintf(L"ciao a tutti\n");

	return 0;
}


