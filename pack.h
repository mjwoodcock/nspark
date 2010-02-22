/*
 * pack/unpack function prototypes
 *
 * $Header: pack.h 1.2 92/11/09 $
 * $Log:	pack.h,v $
 * Revision 1.2  92/11/09  14:49:02  duplain
 * Added putc_init() function prototype.
 * 
 * Revision 1.1  92/09/29  18:02:40  duplain
 * Initial revision
 * 
 */

#ifndef __PACK_H
#define __PACK_H

#include "cproto.h"

void putc_init P__((void));
void putc_ncr P__((FILE *ofp, Byte byte));
Status unpack P__((Header *header, FILE *ifp, FILE *ofp));

#endif /* __PACK_H */
