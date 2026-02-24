/*

  HELPER.H
  ========

*/


#ifndef PG_SOCK_HELP
#define PG_SOCK_HELP


#include <basetsd.h>             /*  for ssize_t data type  */

#define LISTENQ        (1024)   /*  Backlog for listen()   */


/*  Function declarations  */

SSIZE_T Readline(int fd, void *vptr, SIZE_T maxlen);
SSIZE_T Writeline(int fc, const void *vptr, SIZE_T maxlen);


#endif  /*  PG_SOCK_HELP  */

