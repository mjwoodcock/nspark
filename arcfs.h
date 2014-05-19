/*
 * ArcFS support
 * (C) 1993 Andrew Brooks, arb@comp.lancs.ac.uk
 */

#ifndef __ARCFS_H
#define __ARCFS_H

/*
 * Flag to indicate whether the current archive is ArcFS format
 */
extern int arcfs;


/*
 * Number of bits of compression
 */
extern int arcfs_maxbits;


/*
 * Original size of file being extracted
 */
extern long writesize;
extern long crcsize;


/*
 * Read ArcFS header
 */
Header *arcfs_read_header(FILE *ifp);


#endif /* __ARCFS_H */
