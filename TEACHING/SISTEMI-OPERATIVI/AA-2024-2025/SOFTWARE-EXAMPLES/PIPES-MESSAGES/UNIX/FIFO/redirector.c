#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int main (int argc, char *argv[]) {

  int fd;
  int ret;

  if (argc!= 2) {
    printf("Syntax: write_on_fifo <fifo_name>\n");
    exit(-1);
  }

  ret = mkfifo(argv[1], S_IRUSR|S_IWUSR);

  if (ret == -1) {
    perror("fifo creation error: \n");
    exit(EXIT_FAILURE);
  }

  fd = open(argv[1], O_WRONLY);

  if (fd == -1) {
    perror("fifo open error: \n");
    exit(EXIT_FAILURE);
  }

  printf("fd=%d\n",fd);
  fflush(stdout);
  close (1);
  dup(fd);
  execve("./writer", NULL, NULL);
  perror("Exec error: ");
  exit(EXIT_FAILURE);
}

