/*
 * file input/output function prototypes
 *
 * BB changed name to nsparkio.h because of conflict with
 * Borland C++'s io.h
 * $Header: nsparkio.h 1.4 95/06/23 $
 * $Log:    nsparkio.h,v $
 *
 * Revision 1.4  95/06/23  xx:xx:xx  BB
 * Changed name to nsparkio.h for use with Borland C++
 *
 * Revision 1.3  92/12/09  11:41:20  duplain
 * #ifdef'd out write_halfword() and write_word().
 *
 * Revision 1.2  92/10/07  10:37:46  duplain
 * Changed order of function prototypes in line with io.c r1.5 .
 *
 * Revision 1.1  92/09/29  18:02:35  duplain
 * Initial revision
 *
 */

/* BB changed next two lines */
/* #ifndef __IO_H */
/* #define __IO_H */
#ifndef __NSPARKIO_H
#define __NSPARKIO_H

#include "cproto.h"

Ferror check_stream P__((FILE *fp));
Byte read_byte P__((FILE *ifp));
Halfword read_halfword P__((FILE *ifp));
Word read_word P__((FILE *ifp));
void write_byte P__((FILE *ofp, Byte byte));
#ifdef notyet
void write_halfword P__((FILE *ofp, Halfword halfword));
void write_word P__((FILE *ofp, Word word));
#endif /* notyet */
Header *read_header P__((FILE *ifp));

/* BB changed next line */
/* #endif */ /* __IO_H */
#endif /* __NSPARKIO_H */
