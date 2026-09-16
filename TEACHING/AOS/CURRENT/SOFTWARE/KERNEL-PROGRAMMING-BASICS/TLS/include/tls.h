#ifdef TLS

#include <sys/mman.h>
#include <asm/prctl.h>
#include <sys/prctl.h>

#define PER_THREAD_MEMORY_START typedef struct _TLS_ZONE{\
				unsigned long _tls_address;
#define PER_THREAD_MEMORY_END }TLS_ZONE;\
			int TLS_SIZE = sizeof(TLS_ZONE); 

unsigned long tls_position(){
        unsigned long addr;
        asm("mov $0x0, %%rax ; mov %%gs: (%%rax), %%rbx": "=b"  (addr)::);
        return addr;
}

#define READ_THREAD_VARIABLE(var)\
	((TLS_ZONE*)(tls_position()))->var

#define WRITE_THREAD_VARIABLE(var,value)\
	((TLS_ZONE*)(tls_position()))->var = value

#endif
