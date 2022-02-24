#ifndef __HEADER__
#define __HEADER__

#define ERRORE -1
#define FLAG    0
#define SIZE  128


typedef struct {
    int response_channel;
    int service_code;
} request;


typedef struct {
    long mtype;
    request req;
} request_msg;


typedef struct {
    long mtype;
    char mtext[SIZE];
} response_msg;

#endif

