#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mount.h>


#define PAGE_SIZE 4096
#define STACK_SIZE 256*PAGE_SIZE
static char child_stack[STACK_SIZE];

static int child_fn() {
  int ret;

  //this call to unshare is redundant since 
  //this thread has been already activated
  //in a new mount name space
  ret = unshare(CLONE_NEWNS);
  printf("ret is %d - errno is %d\n",ret,errno);

  mount("none", "/", NULL, MS_PRIVATE | MS_REC, NULL);

  printf("PID: %ld\n", (long)getpid());
  execlp("/bin/bash","/bin/bash/",NULL);
  return 0;
}

int main() {
  int ret;

  pid_t child_pid = clone(child_fn, child_stack+STACK_SIZE, CLONE_NEWNS | CLONE_NEWPID | SIGCHLD, NULL);
  printf("clone returned %ld\n", (long)child_pid);

  waitpid(child_pid, NULL, 0);
  return 0;
}
