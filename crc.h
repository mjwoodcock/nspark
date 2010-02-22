/*
 * Cyclic Redundancy Checking function prototypes
 *
 * $Header: crc.h 1.1 92/09/29 $
 * $Log:	crc.h,v $
 * Revision 1.1  92/09/29  18:02:32  duplain
 * Initial revision
 * 
 */

#ifndef __CRC_H
#define __CRC_H

#include "cproto.h"

extern Word crc;
void calccrc P__((Byte byte));

#endif /* __CRC_H */
