/*
 * miscellaneous function prototypes
 *
 * $Header: misc.h 1.4 92/12/08 $
 * $Log:	misc.h,v $
 * Revision 1.4  92/12/08  10:20:19  duplain
 * Added append_type() prototype.
 * 
 * Revision 1.3  92/10/07  10:57:23  duplain
 * Added "#define riscos_path" for RISCOS systems.
 * 
 * Revision 1.2  92/09/30  10:27:19  duplain
 * Added riscos_path().
 * 
 * Revision 1.1  92/09/29  18:02:38  duplain
 * Initial revision
 * 
 */

#ifndef __MISC_H
#define __MISC_H

#include "cproto.h"

char *basename(char *s);
char *name_dot_arc(char *s);
#ifndef RISCOS
char *riscos_path(char *s);
#else /* RISC OS */
#define riscos_path(s) s
#endif /* not RISC OS */
char *uplevel(void);
char *downlevel(char *filename);
void print_details(Header *header);
int inlist(char *filename);
int append_type(Header *header, char *filename);

#ifdef DEBUGGING
void print_header(Header *header);
#endif /* DEBUGGING */

#endif /* __MISC_H */
