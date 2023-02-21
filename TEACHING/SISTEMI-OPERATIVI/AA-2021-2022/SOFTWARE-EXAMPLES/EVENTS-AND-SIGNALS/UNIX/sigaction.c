#include <stdio.h>
#include <signal.h>
#include <unistd.h>


#define SLEEP_PERIOD 5

void generic_handler(int signal, siginfo_t * a, void *b){

  sigset_t set;

  printf("received signal is %d\n",signal);
  fflush(stdout);

}


int main(int argc, char **argv){

  int  i;
  sigset_t set;
  struct sigaction act;


  sigfillset(&set);

  act.sa_sigaction = generic_handler; 
  act.sa_mask =  set;
  act.sa_flags = SA_SIGINFO;
  act.sa_restorer = NULL;
  sigaction(SIGINT,&act,NULL);



  sigprocmask(SIG_BLOCK,&set,NULL);

  while(1) {
	sleep(SLEEP_PERIOD);
	sigpending(&set);
	if(sigismember(&set,SIGINT)){
	  sigemptyset(&set);
	  sigaddset(&set,SIGINT);
	  sigprocmask(SIG_UNBLOCK,&set,NULL);
	  sigprocmask(SIG_BLOCK,&set,NULL);
	}
  }

}
