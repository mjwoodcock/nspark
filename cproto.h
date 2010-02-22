/*
 * protect non-ANSI C Compilers from function prototype arguments
 *
 * $Header: cproto.h 1.2 95/06/28 $
 * $Log:	cproto.h,v $
 * Revision 1.2  95/07/13  xx:xx:xx  BB
 * Added support for Borland C++
 *
 * Revision 1.1  92/09/29  18:02:31  duplain
 * Initial revision
 * 
 */

#ifndef __CPROTO_H
#define __CPROTO_H

/* BB changed next line. Borland C/C++ is not being used in stdc
   mode because some of the extentions (huge pointers) are needed. */
/* #if defined (__STDC__) || defined(__cplusplus) */
#if defined(__STDC__) || defined(__cplusplus) || defined(__MSDOS__)
#define P__(s)	s
#define ANSI_C
#else /* PCC compiler */
#define P__(s)	()
#undef ANSI_C
#endif /* ANSI compiler */

#endif /* __CPROTO_H */
