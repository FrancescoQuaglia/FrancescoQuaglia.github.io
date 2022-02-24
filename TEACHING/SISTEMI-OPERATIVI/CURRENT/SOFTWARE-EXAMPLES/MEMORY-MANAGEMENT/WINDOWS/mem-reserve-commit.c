#include <windows.h>

#define PAGE_SIZE 4096

char buff[10 * PAGE_SIZE] = { 1 }; //just to move the break


int main(int argc, _TCHAR* argv[])
{

	unsigned int page_address = (unsigned int)buff;
	void* p;

	while (1){
		p = VirtualAlloc((LPVOID)(page_address), PAGE_SIZE, MEM_RESERVE, PAGE_READWRITE);

		if (p){
			//			printf("boundary was at %p - page requested address %p - initial guess is at %p - main text is at %p - diff is %d", p, page_address,buff,main,(char*)buff-(char*)main);
			printf("boundary was at %p - page requested address %p - initial guess is at %p - main text is at %p\n", p, page_address, buff, main);
			fflush(stdout);
			break;
		}
		else{
			printf("failure\n");
		}
		page_address += 4096;
	}

	if (argc > 1)
	if (strcmp(argv[1], "commit")==0){
		VirtualAlloc((LPVOID)(p), PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE);
	}

	scanf("%s", (char*)p); //segfault if memory is not committed

	printf("%s\n", (char*)p);
	fflush(stdout);

	 
	if (argc > 2)
	if ( (strcmp(argv[2], "resetundo") == 0)){
		VirtualAlloc((LPVOID)(p), PAGE_SIZE, MEM_RESET, PAGE_READWRITE);
		VirtualAlloc((LPVOID)(p), PAGE_SIZE, MEM_RESET_UNDO, PAGE_READWRITE);
	}

	if (argc > 2)
	if (strcmp(argv[2], "commit")==0){
		VirtualFree((LPVOID)(p), PAGE_SIZE, MEM_DECOMMIT);
		VirtualAlloc((LPVOID)(p), PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE);
	}

	printf("%s\n", (char*)p);
	fflush(stdout);

	return 0;
}

