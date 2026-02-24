
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define COMMAND_LENGTH 1024

int count_args(char *start_command, int *pipe_present, char **next_command) {

	char copied_string[COMMAND_LENGTH + 1];
	char *temp;
	int args = 0, i;

	strcpy(copied_string, start_command);

	if (((temp = strtok(copied_string, " \n")) == NULL) || (strcmp(temp, "\0") == 0)) return(-1);
	args++;

	while (((temp = strtok(NULL, " \n")) != NULL) && (strcmp(temp, "|") != 0)) {
		if (strcmp(temp, "\0") == 0) break;
		args++;
	}
	if (temp == NULL) {
		*pipe_present = 0;
		*next_command = NULL;
	}
	else {
		if ((temp = strtok(NULL, " \n")) != NULL) {
			*pipe_present = 1;
			for (i = 0; ; i++) if (copied_string[i] == '|') break;
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

	arg_vector = (char**)malloc((arg_num + 1) * sizeof(char *));
	for (i = 0; i < arg_num; i++) {
		if (i == 0) temp = strtok(command, " \n");
		else temp = strtok(NULL, " \n");
		arg_vector[i] = temp;
	}
	arg_vector[i] = NULL;
	return(arg_vector);
}

void build_command_line(char **arg, char *line, int num) {
	int i;

	for (i = 0; i < num; i++) {
		sprintf(line, "%s", arg[i]);
		line += strlen(arg[i]);
		*line = ' ';
		line += 1;
	}
	*(line - 1) = '\0';
	return;
}

int main()
{
	char *command_pointer, *next_command;
	char line[COMMAND_LENGTH + 1];
	char command_line[COMMAND_LENGTH + 1];
	char **arg;
	HANDLE *old_pipe_handles;
	HANDLE *new_pipe_handles;
	SECURITY_ATTRIBUTES security;
	HANDLE temp_readHandle;
	HANDLE temp_writeHandle;
	HANDLE pending_processes_handles[MAXIMUM_WAIT_OBJECTS];
	BOOL newprocess;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	int arg_num;
	int pipe_present = 0, pipe_pending = 0;
	int pending_processes;

	while (1) {
		printf("\nExample Shell>");
		fflush(stdout);
		fgets(line, COMMAND_LENGTH, stdin);
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
			build_command_line(arg, command_line, arg_num);
			if (pipe_present) {
				new_pipe_handles = (HANDLE*)malloc(sizeof(HANDLE) * 2);
				security.nLength = sizeof(security);
				security.lpSecurityDescriptor = NULL;
				security.bInheritHandle = TRUE;
				if (CreatePipe(&new_pipe_handles[0], &new_pipe_handles[1], &security, 0) < 0) {
					printf("Can't open a pipe for error %d!\n", GetLastError()); fflush(stdout);
					ExitProcess(-1);
				}
			}

			if (pipe_pending) {
				temp_readHandle = GetStdHandle(STD_INPUT_HANDLE);
				SetStdHandle(STD_INPUT_HANDLE, old_pipe_handles[0]);
			}
			if (pipe_present) {
				temp_writeHandle = GetStdHandle(STD_OUTPUT_HANDLE);
				SetStdHandle(STD_OUTPUT_HANDLE, new_pipe_handles[1]);
			}

			memset(&si, 0, sizeof(si));
			memset(&pi, 0, sizeof(pi));
			si.cb = sizeof(si);

			newprocess = CreateProcess(NULL,
				command_line,
				NULL, NULL,
				TRUE,
				NORMAL_PRIORITY_CLASS,
				NULL, NULL,
				&si, &pi);


			if (pipe_pending) {
				SetStdHandle(STD_INPUT_HANDLE, temp_readHandle);
				CloseHandle(old_pipe_handles[0]);
				free(old_pipe_handles);
				pipe_pending = 0;
			}

			if (pipe_present) {
				SetStdHandle(STD_OUTPUT_HANDLE, temp_writeHandle);
				CloseHandle(new_pipe_handles[1]);
				old_pipe_handles = new_pipe_handles;
				pipe_pending = 1;
			}

			if (newprocess == 0) {
				printf("Can't spawn executable %s for command \"%s\" (error %d)!\n", arg[0], command_line, GetLastError());
				break;
			}
			pending_processes_handles[pending_processes] = pi.hProcess;
			pending_processes++;
			command_pointer = next_command;
		} while (next_command != NULL);

		WaitForMultipleObjects(pending_processes, pending_processes_handles, TRUE, INFINITE);

	}
	printf("\nLeaving Shell\n\n");
	return(0);
}
