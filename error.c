
/*
 * report errors and warnings
 *
 * $Header: error.c 1.2 92/12/07 $
 * $Log:	error.c,v $
 * Revision 1.2  92/12/07  17:18:02  duplain
 * reformatted source.
 * 
 * Revision 1.1  92/09/29  18:02:18  duplain
 * Initial revision
 * 
 */

#include <stdio.h>
#include "error.h"

extern char *ourname;			/* in main.c */

void
error(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
#ifdef BSD42
	_doprnt(fmt, ap, stderr);
#else
	vfprintf(stderr, fmt, ap);
#endif
	putc('\n', stderr);
	va_end(ap);

}

void
warning(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, "%s warning: ", ourname);
#ifdef BSD42
	_doprnt(fmt, ap, stderr);
#else
	vfprintf(stderr, fmt, ap);
#endif							/* BSD42 */
	putc('\n', stderr);
	va_end(ap);
}
