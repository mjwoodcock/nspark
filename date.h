/*
 * manipulate date function prototypes
 *
 * $Header: date.h 1.2 92/10/23 $
 * $Log:	date.h,v $
 * Revision 1.2  92/10/23  14:09:57  duplain
 * Removed "days_to_month" function prototype.
 * 
 * Revision 1.1  92/09/29  18:02:33  duplain
 * Initial revision
 * 
 */
 
#ifndef __DATE_H
#define __DATE_H

Date *makedate(Header *header);
char *monthname(int month);
		     
#endif /* __DATE_H */

