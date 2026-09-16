#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

//all system calls will fail if this program is not setuid-root or is not run by the root user

int main(void) {
	
    int ret; 

    printf("--------------------\n");
    printf("uid %d - euid %d\n",getuid(),geteuid());

    seteuid(700);
    printf("--------------------\n");
    printf("uid %d - euid %d\n",getuid(),geteuid());

    seteuid(800);//this will deterministically fail
    printf("--------------------\n");
    printf("uid %d - euid %d\n",getuid(),geteuid());

    seteuid(0);
    printf("--------------------\n");
    printf("uid %d - euid %d\n",getuid(),geteuid());

    setuid(10);
    printf("--------------------\n");
    printf("uid %d - euid %d\n",getuid(),geteuid());

    return 0;

}



