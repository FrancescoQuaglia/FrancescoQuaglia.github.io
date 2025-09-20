
/* redirect the standard output on a file
 * then objdump the fiel to inspect what's there in
 * do not forget to shut down the  paused program by, e.g. CTL+C
 */

#include <sys/auxv.h>
#include <stdio.h>
#include <unistd.h>

#define PAGE_SIZE 4096

#define AUDIT if(0)

int main(int argc, char** argv){

	void* p = (void*) (uintptr_t) getauxval(AT_SYSINFO_EHDR);
	
	AUDIT{
	printf("VDSO addr is %p\n",p);
	fflush(stdout);
	}
	
	write(1,p,4*PAGE_SIZE);

	pause();

}
