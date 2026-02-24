/*

  HELPER.C
  ========
  
*/

//#include "helper.h"

#include <winsock.h>
#include <basetsd.h>

/*  Read a line from a socket  */

SSIZE_T Readline(int sockd, void *vptr, SIZE_T maxlen)
{
    SSIZE_T n, rc;
    char    c, *buffer;

    buffer = vptr;

    for ( n = 1; n < maxlen; n++ )
	{
		rc = recv(sockd, &c, 1, 0);
		if ( rc == 1 )
		{
	    	*buffer++ = c;
	    	if ( c == '\n' ) break;
		}
		else
			if ( rc == 0 )
			{
			    if ( n == 1 ) return 0;
			    else break;
			}
			else return -1;
    }
    *buffer = 0;
    return n;
}


/*  Write a line to a socket  */

SSIZE_T Writeline(int sockd, const void *vptr, SIZE_T n)
{
    SIZE_T      nleft;
    SSIZE_T     nwritten;
    const char *buffer;

    buffer = vptr;
    nleft  = n;

    while ( nleft > 0 )
	{
		if ( (nwritten = send(sockd, buffer, nleft, 0)) <= 0 ) return -1;
		nleft  -= nwritten;
		buffer += nwritten;
    }
    return n;
}
