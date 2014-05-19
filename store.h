/*
 * store/unstore function prototypes
 *
 * $Header: store.h 1.1 92/09/29 $
 * $Log:	store.h,v $
 * Revision 1.1  92/09/29  18:02:42  duplain
 * Initial revision
 * 
 */

#ifndef __STORE_H
#define __STORE_H

Status unstore(Header *header, FILE *ifp, FILE *ofp);

#endif /* __STORE_H */
