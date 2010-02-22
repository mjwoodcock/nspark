/*
 * global variable declarations
 *
 * $Header: main.h 1.4 92/12/08 $
 * $Log:	main.h,v $
 * Revision 1.5  93/08/20  11:35:10  arb
 * Added reference to singlecase variable.
 *
 * Revision 1.4  92/12/08  10:19:54  duplain
 * Added reference to apptype variable.
 * 
 * Revision 1.3  92/10/01  11:21:57  duplain
 * Added reference to retry variable.
 *
 * Revision 1.3 99/03/17   MU
 * Added inffiles
 * 
 * Revision 1.2  92/09/30  10:27:42  duplain
 * Added logfile.
 * 
 * Revision 1.1  92/09/29  18:02:36  duplain
 * Initial revision
 * 
 */

#ifndef __MAIN_H
#define __MAIN_H

extern char *ourname;
extern char *archive;
extern char *logfile;
extern char **files;
extern unsigned char unarc;
extern unsigned char inffiles;
extern unsigned char quiet;
extern unsigned char verbose;
extern unsigned char testing;
extern unsigned char listing;
extern unsigned char force;
extern unsigned char stamp;
extern unsigned char retry;
extern unsigned char apptype;
extern unsigned char singlecase;
#ifdef DEBUGGING
extern unsigned char debugging;
#endif /* DEBUGGING */

#endif /* __MAIN_H */
