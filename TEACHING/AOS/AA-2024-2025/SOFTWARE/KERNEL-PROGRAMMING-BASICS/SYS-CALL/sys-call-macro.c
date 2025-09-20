#include <unistd.h>

#define __NR_my_fork 2

#define _new_syscall0(name) \
int name(void) \
{ \
 asm("int $0x80" : : "a" (__NR_##name) ); \
 return 0; \
} \

_new_syscall0(my_fork)

int main(int a, char** b){

	my_fork();

	pause();

}
