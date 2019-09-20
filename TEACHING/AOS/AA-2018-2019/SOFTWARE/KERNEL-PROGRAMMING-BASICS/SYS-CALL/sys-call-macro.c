#include <unistd.h>
//#include <asm/unistd.h>

#define __NR_my_fork 2

#define _new_syscall0(name) \
int name(void) \
{ \
 asm("int $0x80" : : "a" (__NR_##my_fork) ); \
 return 0; \
} \

//_syscall(int,fork)

_new_syscall0(my_fork)

int main(int a, char** b){

	my_fork();

	pause();

}
