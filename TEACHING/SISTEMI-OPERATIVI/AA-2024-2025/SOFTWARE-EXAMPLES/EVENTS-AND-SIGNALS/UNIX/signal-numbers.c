#include <stdio.h>
#include <signal.h>
#include <unistd.h>


void generic_handler(int signal){

	printf("received signal is %d\n",signal);
	fflush(stdout);
}



int main(int argc, char **argv){

  int  i;

  signal(SIGINT,generic_handler);
  signal(SIGUSR1,generic_handler);
  signal(SIGUSR2,generic_handler);

  while(1) {
	pause();
  }

}
