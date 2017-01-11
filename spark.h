/*
 * defines types used in nspark
 *
 * $Header: spark.h 1.8 95/08/01 $
 * $Log:	spark.h,v $
 * Revision 1.8  95/08/01  xx:xx:xx  BB
 * Added MSDOS6 and MSDOS7 macros
 *
 * Revision 1.7  94/11/09  10:36:00  auj
 * Added Windows NT support.
 *
 * Revision 1.6  93/04/05  12:34:56  arb
 * Added RISC OS support.
 *
 * Revision 1.5  92/11/04  16:57:32  duplain
 * Completed CT_xxx types.
 * 
 * Revision 1.4  92/10/23  14:06:57  duplain
 * Added test for Sys V.4 when defining Sys V file and path name lengths.
 * 
 * Revision 1.3  92/10/06  12:22:11  duplain
 * Removed "days_since_1900" from struct date.
 * 
 * Revision 1.2  92/09/30  10:28:13  duplain
 * Added W_OPENMODE and R_OPENMODE.
 * 
 * Revision 1.1  92/09/29  18:02:41  duplain
 * Initial revision
 * 
 */

#ifndef __SPARK_H
#define __SPARK_H

/*
 * globalise system-type defines...
 */
#if defined(RISCOS2) || defined(RISCOS3)
#define RISCOS
#define NS_LITTLE_ENDIAN
#endif
/* BB changed next line */
/* #if defined(MSDOS2) || defined(MSDOS3) || defined(MSDOS4) || defined(MSDOS5) || defined(WINNT) */
#if defined(MSDOS2) || defined(MSDOS3) || defined(MSDOS4) || \
    defined(MSDOS5) || defined(MSDOS6) || defined(MSDOS7) || \
    defined(WINNT)
#define MSDOS
#define NS_LITTLE_ENDIAN
#endif

/*
 * "Word" must be a 4-byte type.
 * "Halfword" must be a 2-byte type.
 * "Byte" must be a 1-byte type.
 */
#if defined (MSDOS) && !defined(WINNT)
typedef unsigned long	Word;
typedef	unsigned int	Halfword;
typedef unsigned char	Byte;
#else /* not MSDOS */
typedef unsigned int	Word;
typedef unsigned short	Halfword;
typedef unsigned char	Byte;
#endif /* MSDOS */

/*
 * define the path seperator character, and file open mode string.
 */
#if defined(POSIX)
#define PATHSEP	'/'
#define PATHSEPSTR	"/"
#define W_OPENMODE	"w"
#define R_OPENMODE	"r"
#endif
#if defined(RISCOS)
#define PATHSEP	'.'
#define PATHSEPSTR	"."
#define W_OPENMODE	"w"
#define R_OPENMODE	"r"
#endif
#if defined(MSDOS)
#if defined(WINNT)
#define PATHSEP   '/'       /* Win NT uses \ as the seperator, but MSC
                               allows /, so use that as it makes life
                               easier in filename translation. */
#define PATHSEPSTR    "/"
#else
#define PATHSEP	'\\'
#define PATHSEPSTR	"\\"
#endif
#define W_OPENMODE	"wb"
#define R_OPENMODE	"rb"
#endif

/*
 * define maximum filename and pathname length
 */
#if defined(POSIX)
#define FILENAMELEN	256
#define PATHNAMELEN	1024
#endif
#if defined (RISCOS)
#define FILENAMELEN	10
#define PATHNAMELEN	256
#endif
#if defined (MSDOS)
#if defined (WINNT)
#define FILENAMELEN 256
#define PATHNAMELEN 1024
#else
#define FILENAMELEN	12		/* including dot */
#define PATHNAMELEN	256
#endif
#endif

/*
 * stream error enumeration
 */
typedef enum {FNOERR, FEND, FRWERR} Ferror;

/*
 * file type
 */
typedef enum {NOEXIST, ISFILE, ISDIR } Ftype;

/*
 * un-archiving status
 */
typedef enum {NOERR, RERR, WERR, CRCERR } Status;

/*
 * compressed file header
 */
typedef struct {
	Byte comptype;		/* compression type */
	char name[14];		/* name of file */
	Word complen;		/* compressed length */
	Halfword date;		/* file creation date */
	Halfword time;		/* file creation time */
	Halfword crc;		/* Cyclic Redundancy Check */
	Word origlen;		/* original length */
	Word load;		/* load address */
	Word exec;		/* exec address */
	Word attr;		/* file attributes */
} Header;

/*
 * universal date structure
 */
typedef struct {
	int second;
	int minute;
	int hour;
	int day;
	int month;
	int year;
} Date;
		
/*
 * compress type (passed to uncompress())
 */
typedef enum { COMPRESS, SQUASH, CRUNCH, UNIX_COMPRESS } CompType;

/*
 * compression types
 */
#define CT_NOTCOMP	(Byte)0x01	/* not compressed (old) */
#define CT_NOTCOMP2	(Byte)0x02	/* not compressed (new) */
#define CT_PACK		(Byte)0x03	/* run length */
#define CT_PACKSQUEEZE	(Byte)0x04	/* run length + Huffman squeezing */
#define CT_LZOLD	(Byte)0x05	/* Lempel-Ziv (old) */
#define CT_LZNEW	(Byte)0x06	/* Lempel-Ziv (new) */
#define CT_LZW		(Byte)0x07	/* Lempel-Ziv Welch */
#define CT_CRUNCH	(Byte)0x08	/* Dynamic LZW with adaptive reset */
#define CT_SQUASH	(Byte)0x09	/* PKARC squashing */
#define CT_COMP		(Byte)0x7f	/* UNIX compress */

/*
 * other spark defines
 */
#define STARTBYTE	(Byte)0x1a	/* start of archive marker */
#define RUNMARK		(Byte)0x90	/* start of run (pack/unpack) */
#define ARCHPACK	(Byte)0x80	/* bit-7 set in comptype if Archie */

#endif /* __SPARK_H */
