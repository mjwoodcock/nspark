/*
 * compress/uncompress function prototypes
 *
 * $Header: compress.h 1.1 92/09/29 $
 * $Log:	compress.h,v $
 * Revision 1.1  92/09/29  18:02:30  duplain
 * Initial revision
 * 
 */

#ifndef __COMPRESS_H
#define __COMPRESS_H

#include "cproto.h"

Status uncompress P__((Header *header, FILE *ifp, FILE *ofp, CompType type));

#endif /* __COMPRESS_H */
