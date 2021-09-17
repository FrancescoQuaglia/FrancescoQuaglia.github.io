#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CHAR	1000
#define MAX_STRING      2000

int string_number=0;

char * string_array[MAX_STRING];

void *status;

void * OrderThread(void * new_string) {

  int i,j;
  long int status;

  //printf("Ordering thread active\n");
  for (i=0 ; i < string_number; i++) {
    if (strcmp(new_string,string_array[i]) <= 0) {
      for (j=0; j<(string_number-i); j++) {
	string_array[string_number - j] = string_array[string_number - j - 1];
      }
      break;
    }
  }
  string_array[i] = new_string;
  string_number++;
  status = 0;
  pthread_exit((void *)status);
} 

int main () {
  int i;
  pthread_t tid = 0;
  char * old_buffer;
  void * status;
  char buffer[MAX_CHAR];
  int notfirst = 0;

  while(1) {
    printf("Insert string: ");
    scanf("%s", buffer);

    if (strcmp(buffer, "quit") == 0 && notfirst == 0) exit(0);
    if (strcmp(buffer, "quit") == 0) break;
    old_buffer = strdup(buffer);

    if (notfirst) pthread_join(tid, &status);
    else notfirst = 1;
    i=pthread_create(&tid, NULL, OrderThread, (void *)old_buffer);
    if (i) {
      printf("cannot create thread for error %d\n", i);
      exit(-1);
      }
  }
  pthread_join(tid, &status);
  for (i=0; i< string_number; i++) printf("String %d: %s\n", i,string_array[i]);
  exit(0);
}

