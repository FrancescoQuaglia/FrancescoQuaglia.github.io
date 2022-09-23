#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>



int main(void) {
	
    int ret; 

    printf("--------------------\n",ret);
    printf(
        "         UID           GID  \n"
        "Real      %d  Real      %d  \n"
        "Effective %d  Effective %d  \n",
             getuid (),     getgid (),
             geteuid(),     getegid()
    );


#ifdef VARIANT
    printf("--------------------\n",ret);
    ret = setuid(600); 
    printf("ret is %d\n",ret);
    printf(
        "         UID           GID  \n"
        "Real      %d  Real      %d  \n"
        "Effective %d  Effective %d  \n",
             getuid (),     getgid (),
             geteuid(),     getegid()
    );
#endif


    printf("--------------------\n",ret);
ret = seteuid(700);
    printf("ret is %d\n",ret);
    printf(
        "         UID           GID  \n"
        "Real      %d  Real      %d  \n"
        "Effective %d  Effective %d  \n",
             getuid (),     getgid (),
             geteuid(),     getegid()
    );

    printf("--------------------\n",ret);
ret = setuid(0);
    printf("ret is %d\n",ret);

    printf(
        "         UID           GID  \n"
        "Real      %d  Real      %d  \n"
        "Effective %d  Effective %d  \n",
             getuid (),     getgid (),
             geteuid(),     getegid()
    );

    printf("--------------------\n",ret);
ret = seteuid(1000);
    printf("ret is %d\n",ret);

    printf(
        "         UID           GID  \n"
        "Real      %d  Real      %d  \n"
        "Effective %d  Effective %d  \n",
             getuid (),     getgid (),
             geteuid(),     getegid()
    );

    printf("--------------------\n",ret);
ret = setuid(0); // HOW DOES THIS SUCCEED IN SETTING THE EUID BACK TO 0?
    printf("ret is %d\n",ret);
    printf(
        "         UID           GID  \n"
        "Real      %d  Real      %d  \n"
        "Effective %d  Effective %d  \n",
             getuid (),     getgid (),
             geteuid(),     getegid()
    );

    return 0 ;       
}



