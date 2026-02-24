#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>


void generic_handler(int signal, siginfo_t *info, void* unused){

	sigset_t set;

	printf("received signal is %d - address is %p\n",signal,info->si_addr);
	fflush(stdout);
	sleep(1);
}


int main(int argc, char **argv){

  int  i;
  char c;
  sigset_t set;
  struct sigaction act;
  char* addr = (char*)0xffffff00;


  sigfillset(&set);
  sigdelset(&set,SIGSEGV);
  sigdelset(&set,SIGINT);

  act.sa_sigaction = generic_handler; 
  act.sa_mask =  set;
  act.sa_flags = SA_SIGINFO;
  act.sa_restorer = NULL;
  sigaction(SIGSEGV,&act,NULL);


  while(1) {
	sleep(5);
	c = *addr;
  }
}
