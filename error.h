/*
 * error/warning report function prototypes
 *
 * $Header: error.h 1.2 92/12/07 $
 * $Log:	error.h,v $
 * Revision 1.2  92/12/07  17:25:36  duplain
 * Changed include files from "xxxarg.h" to <xxxarg.h> .
 * 
 * Revision 1.1  92/09/29  18:02:34  duplain
 * Initial revision
 * 
 */

#ifndef __ERROR_H
#define __ERROR_H

#include "cproto.h"

#ifdef ANSI_C
#include <stdarg.h>
#else
#include <varargs.h>
#endif

void error P__((char *fmt, ...));
void warning P__((char *fmt, ...));

#endif /* __ERROR_H */
