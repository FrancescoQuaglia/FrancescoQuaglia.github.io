
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <sys/wait.h>
#include <stdlib.h>

#define COMMAND_LENGTH 1024

int count_args(char *start_command, int *pipe_present, char **next_command) {
    char copied_string[COMMAND_LENGTH + 1];
    char *temp, *temp_next;
    int args = 0, i;

    strcpy(copied_string, start_command);
    if (((temp = strtok(copied_string, " \n")) == NULL) || (strcmp(temp, "\0") == 0)) return(-1);
    args++;
    while (((temp = strtok(NULL, " \n")) != NULL) && (strcmp(temp, "|") != 0 )) {
	    if (strcmp(temp, "\0") == 0 ) break;
	    args++;
    }
    if (temp == NULL) {
	    *pipe_present = 0;
	    *next_command = NULL;
    } else {
	if ((temp = strtok(NULL, " \n")) != NULL) {
	  *pipe_present = 1;
          for (i=0; ; i++) if (copied_string[i] == '|') break;
	  *next_command = start_command + i + 2;
	}
	else {
	  *pipe_present = 0;
	  *next_command = NULL;
	}
	
    }
    return(args);
}

char **build_arg_vector(char *command, int arg_num, int pipe_present) {
  char **arg_vector;
  char *temp;
  int i;

  arg_vector = malloc((arg_num+1) * sizeof(char *));
  for(i=0; i < arg_num; i++) {
    if (i==0) temp = strtok(command, " \n");
    else temp = strtok(NULL, " \n");
    arg_vector[i] = temp;
  }
  arg_vector[i] = NULL;
  return(arg_vector);
}

int main () {

	char *command_pointer, *next_command;
	char line[COMMAND_LENGTH+1];
	char **arg;
	int *old_pipe_descriptors;
	int *new_pipe_descriptors;
	int arg_num;
	int pipe_present=0, pipe_pending=0;
	int pending_processes;
	int i, status;

  	while (1) {
  	printf("\nExample Shell");
   	if (geteuid() == 0) printf("#");
   	else printf(">");
    	fflush(stdout);
    		fgets(line,COMMAND_LENGTH,stdin);
    		line[COMMAND_LENGTH] = '\0';
    		if (strlen(line) == COMMAND_LENGTH) {
      			printf("\nCommand too long!\n");
      			continue;
   		 }

   		 pending_processes = 0;
   		 command_pointer = line;

    		if (strcmp(command_pointer, "exit\n") == 0) break;
   		do {
     			arg_num = count_args(command_pointer, &pipe_present, &next_command);
     			if (arg_num < 0) break;

      			arg = build_arg_vector(command_pointer, arg_num, pipe_present);

      			if(pipe_present) {
				new_pipe_descriptors = malloc(sizeof(int)*2);
       				if (pipe (new_pipe_descriptors) < 0) {
          				printf("Can't open a pipe for error %d!\n", errno);fflush(stdout);
         				 exit(EXIT_FAILURE);
       				}
     			 }
      
      			if ((i=fork()) == 0) {
				if (pipe_pending) {
	 				close(0);
      					dup(old_pipe_descriptors[0]);
          				close(old_pipe_descriptors[0]);
				}
				if (pipe_present) {
	 				 close(1);
	 				 dup(new_pipe_descriptors[1]);
					 close(new_pipe_descriptors[1]);
				}

      				execvp(arg[0],arg);
				printf("Can't execute file %s\n",arg[0]);fflush(stdout);
				exit(EXIT_FAILURE);
      			} else if (i>0) {
        			pending_processes++;
				if (pipe_pending) {
	  			close(old_pipe_descriptors[0]);
	  			free(old_pipe_descriptors);
	 			pipe_pending = 0;
			}
			if (pipe_present) {
	  			close(new_pipe_descriptors[1]);
	  			old_pipe_descriptors = new_pipe_descriptors;
	 			pipe_pending = 1;
			}
      			} else {//else if
       				 printf("Can't spawn process for error %d\n", errno);fflush(stdout);
     			 }
    			command_pointer = next_command;
    		} while (next_command != NULL);

 		for (i=0; i<pending_processes; i++) {
			  wait(&status);
   		 }
 	 }

	printf("\nLeaving the Shell\n\n");
	return(0);

}
