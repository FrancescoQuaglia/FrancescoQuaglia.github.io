#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(){

  int pid;

  scanf("%d",&pid);
  printf("killing %d - res is %d\n",pid,kill(pid,SIGINT));

}
