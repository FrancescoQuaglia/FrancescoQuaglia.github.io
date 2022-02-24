// discovers boundary of data section and other memory layout info
//

#include <windows.h>
#define PAGE_SIZE 4096

char buff[10 * PAGE_SIZE] = { 1 };


int main(int argc, _TCHAR* argv[])
{

	unsigned int page_address = (unsigned int)buff;
	void* p;

	while (1){
		p = VirtualAlloc((LPVOID)(page_address), PAGE_SIZE, MEM_RESERVE, PAGE_READWRITE);
		if (p){
			printf("boundary was at %p - page requested address %p - initial guess is at %p - main text is at %p\n", p, page_address,buff,main);
			break;
		}
		else{
			printf("failure\n");
		}
		page_address += 4096;
	}

	return 0;
}
