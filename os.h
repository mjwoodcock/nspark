/*
 * Operating System specific function prototypes
 *
 * $Header: os.h 1.2 92/10/01 $
 * $Log:	os.h,v $
 * Revision 1.3  93/03/05 15:37:24   arb
 * Added read() function prototype for RISCOS
 *
 * Revision 1.2  92/10/01  11:22:18  duplain
 * Added filesize() function prototype.
 * 
 * Revision 1.1  92/09/29  18:02:39  duplain
 * Initial revision
 * 
 */

#ifndef __OS_H
#define __OS_H

#include "cproto.h"

Word filesize(char *pathname);
Ftype exist(char *pathname);
int makedir(char *pathname);
int filestamp(Header *header, char *filename);

#ifdef RISCOS
int read(int fd, void *buffer, int size); 
#endif
#endif /* __OS_H */
