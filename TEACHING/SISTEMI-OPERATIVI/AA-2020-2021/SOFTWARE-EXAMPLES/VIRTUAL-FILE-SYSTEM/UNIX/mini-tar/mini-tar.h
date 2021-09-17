

#define BUFSIZE 1024
#define MAX_MANAGEABLE_FILES 128
#define MAX_FILE_NAME_LENGTH 128

typedef struct _header{
        int  num_files;
        char file_names[MAX_MANAGEABLE_FILES][MAX_FILE_NAME_LENGTH];
        int  start_position[MAX_MANAGEABLE_FILES];
        int  end_position[MAX_MANAGEABLE_FILES];
} header;

#define AUDIT if(0)
