#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#define SLEEP_PERIOD 5

int main(int argc, char **argv){

  int  i;
  sigset_t set;


  sigfillset(&set);
  sigprocmask(SIG_BLOCK,&set,NULL);

  while(1) {
	sleep(SLEEP_PERIOD);
	printf("querying the sigset\n");
	sigpending(&set);

	if(sigismember(&set,SIGINT)){
	  sigemptyset(&set);
	  sigaddset(&set,SIGINT);
	  sigprocmask(SIG_UNBLOCK,&set,NULL);
	}
  }

}
