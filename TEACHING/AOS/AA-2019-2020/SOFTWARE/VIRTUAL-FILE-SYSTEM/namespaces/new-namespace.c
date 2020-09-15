#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define STACK_SIZE 256*PAGE_SIZE
static char child_stack[STACK_SIZE];

//a simple program for crewating new PID an NS namespaces in combination

static int child_fn() {
  printf("PID: %ld\n", (long)getpid());
//  chroot("/home/francesco/dummy/");//try various combinations on your own file system structure to get what happens
 // chdir("/home/francesco/dummy/");
  execlp("/bin/bash","/bin/bash/",NULL);
  execlp("../../../bin/bash","/bin/bash/",NULL);
  printf("Still here\n");
  return 0;
}

int main() {

  pid_t child_pid = clone(child_fn, child_stack+STACK_SIZE, CLONE_NEWPID | CLONE_NEWNS| SIGCHLD, NULL);
  printf("clone returned %ld\n", (long)child_pid);

  waitpid(child_pid, NULL, 0);
  return 0;
}
