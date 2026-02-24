
#define BUFSIZE 1024
#define MAX_MANAGEABLE_FILES 128
#define MAX_FILE_NAME_LENGTH 128
#define PM_SIZE 12

char * the_magic = "\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa";//this is an arbitrary sequence of values


typedef struct __attribute__((__packed__)) _position {
	char pos[PM_SIZE];
} position;

typedef struct __attribute__((__packed__)) _header{
	char magic[PM_SIZE]; 
	char num_files[PM_SIZE];
        char file_names[MAX_MANAGEABLE_FILES][MAX_FILE_NAME_LENGTH];
        position  start_position[MAX_MANAGEABLE_FILES];
        position  end_position[MAX_MANAGEABLE_FILES];
} header;

#define AUDIT if(1)
