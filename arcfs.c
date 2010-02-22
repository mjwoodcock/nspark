
/*
 * Handle ArcFS format archives
 *
 * Author: Andrew Brooks, arb@comp.lancs.ac.uk
 *
 * $Header: arcfs.c 1.5 95/08/01 $
 * $Log:        arcfs.c,v $
 * Revision 1.5  95/08/01  xx:xx:xx  BB
 * Fixes for Borland C/C++
 * Removed use of floating point routines.
 *
 * Revision 1.4  95/01/06  11:58:45  arb
 * Fixes for Alpha.
 *
 * Revision 1.3  94/12/12  17:25:25  arb
 * Fixes for writesize.
 *
 * Revision 1.2  94/10/26  15:06:35  arb
 * Fixed date and time conversion.
 *
 * Revision 1.1  94/02/28  21:41:23  arb
 * Fixed header, fixed include ordering, fixed directory check,
 * added seek to start of compressed data, fixed maxbits, ...
 * ie. got the thing working at last!
 *
 * Revision 1.0  93/08/20  12:40:15  arb
 * Initial revision
 */

#include <stdio.h>
#include <time.h>
#include "spark.h"
#ifdef UNIX
#include <sys/types.h>
#endif							/* UNIX */

/* BB changed next line */

/* #ifdef RISCOS */
#if defined(RISCOS) || defined(__MSDOS__)
#include <stdlib.h>
#include <string.h>
#endif	/* RISCOS */						/* || __MSDOS__ */
#include "arcfs.h"

/* BB changed next line because of conflict with Borland's io.h. */

/* #include "io.h" */
#include "nsparkio.h"

/* BB added next include for print_header(). */
#ifdef __MSDOS__
#include "misc.h"
#endif							/* __MSDOS__ */

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifdef UNIX
static char rcsid[] = "$Header: arcfs.c 1.5 95/08/01 $";
#endif							/* UNIX */


/*
 * Public flag to indicate whether the current archive is ArcFS format
 */
int arcfs = 0;


/*
 * Public number of compression bits, used in compress.c
 */
int arcfs_maxbits = 0;


/*
 * Public size of file being extracted, used in io.c, crc.c
 */
long writesize;
long crcsize;


/*
 * ArcFS header list element
 */
struct arcfs_header_s
{
	struct arcfs_header_s *next;
	Byte maxbits;
	Byte is_dir;
	Byte info_byte;
	Word info_word;
	Word seek;
	Header *header;
};
typedef struct arcfs_header_s *arcfs_header;


/*
 * Info byte
 */
#define AFS_ENDDIR   0
#define AFS_DELETED  1
#define AFS_STORE    0x82
#define AFS_PACK     0x83
#define AFS_CRUNCH   0x88
#define AFS_COMPRESS 0xFF


/*
 * Static data
 */
static int arcfs_initialised = 0;
static arcfs_header header_list = NULL;
static arcfs_header header_ptr = NULL;


/*
 * Convert RISC OS time to UNIX time.
 * RISC OS time is five bytes of centiseconds since 1900.
 * UNIX time is seconds since 1970.
 * MSB of RISC OS time is LSB of `load' plus `exec'.
 */

/* BB added extra prototype for Borland C/C++ */
#ifdef __MSDOS__
struct tm *rotm P__((Word load, Word exec));
#endif							/* __MSDOS__ */

struct tm *
rotm(load, exec)
	Word load, exec;
{
	Word low, high;
	time_t t;

	high = (load & 0xff) - 0x33l;
	low = exec - 0x6e996a00l;
	/* BB changed constant in next line to long */
	t = (time_t) (high * 42949673L + low / 100L);
	return (localtime(&t));
}


/*
 * Convert RISC OS time to SPARK time
 */

/* BB added extra prototype for Borland C/C++ */
#ifdef __MSDOS__
void arcfs_fixtime P__((Header * hdr));
#endif							/* __MSDOS__ */

void
arcfs_fixtime(hdr)
	Header *hdr;
{
	/* BB next line commented out, variable ti never used */
	/* time_t ti; */
	struct tm *tim;

	/* Convert to UNIX time first (as it is easy) */
	tim = rotm(hdr->load, hdr->exec);

	/* Convert UNIX time to SPARK time */
	hdr->date = (tim->tm_year - 80) << 9;
	hdr->date |= (tim->tm_mon + 1) << 5;
	hdr->date |= (tim->tm_mday);
	hdr->time = (tim->tm_hour) << 11;
	hdr->time |= (tim->tm_min) << 5;
	hdr->time |= (tim->tm_sec);
}


/*
 * Read ArcFS header
 */
Header *
arcfs_read_header(ifp)
	FILE *ifp;
{
	static Header null_header;
	static Word data_start;
	Word header_length = 0;
	Header *header;
	Word version;
	int i;
	Byte info_byte, name[12];
	Word length, load, exec, attr, complen, info_word;
	arcfs_header header_prev;
	int j;

	/* Return next header from list */
	if (arcfs_initialised)
	{
		/* If end of list return an empty header structure to indicate EOF */
		if (header_ptr == NULL)
			return (&null_header);

		/* Return next header in list */
		header = header_ptr->header;
		/* Seek to start of compressed data */
		if ((!header_ptr->is_dir)
			&& (fseek(ifp, (long) header_ptr->seek, SEEK_SET)))
		{
			printf("Cannot seek compressed data in this file\n");
			return (&null_header);
		}
		/* Set up number of compression bits */
		arcfs_maxbits = header_ptr->maxbits;
		/*if (header_ptr->is_dir) header = &null_header; */
		header_ptr = header_ptr->next;
		return (header);
	}

	/* Header list not constructed yet, so read all headers from file */
	arcfs_initialised = 1;
#ifdef BSD
	bzero((char *) &null_header, sizeof(null_header));
#else							/* not BSD */
	memset((char *) &null_header, '\0', sizeof(null_header));
#endif							/* BSD */
	null_header.comptype = 0;
	header_length = read_word(ifp);
	data_start = read_word(ifp);
	if ((version = read_word(ifp)) > 40)
	{
		/* BB removed floating point routines from next line
		   This saves linking the floating point routines under DOS
		   which yields quite a reduction in executable size.
		   And it removes the need to have the FPE present under RISC OS. */
		/* printf("Archive created by a newer version of ArcFS (%.2f)\n",((float)version)/100); */
		printf("Archive created by a newer version of ArcFS (%d.%02d)\n",
			   version / 100, version % 100);
		return (&null_header);
	}
	read_word(ifp);				/* read/write version */
	if ((version = read_word(ifp)) > 0)
	{
		printf("Archive format %d not understood\n", version);
		return (&null_header);
	}
	for (i = 0; i < 17; i++)
		read_word(ifp);			/* reserved */

	/* Read list of headers */
	for (i = 0; i < header_length / 36; i++)
	{
		/* Create list item */
		header = (Header *) malloc(sizeof(Header));
		header_ptr = (arcfs_header) malloc(sizeof(struct arcfs_header_s));
		if ((header == NULL) || (header_ptr == NULL))
		{
			printf("Out of memory\n");
			return (&null_header);
		}

		/* Read ArcFS file header */
		info_byte = read_byte(ifp);
		for (j = 0; j < 11; j++)
		{
			name[j] = read_byte(ifp);
			if (name[j] == PATHSEP)
				name[j] = '_';
			if (name[j] < ' ' || name[j] > '~')
				name[j] = '\0';
		}
		name[j] = '\0';
		length = read_word(ifp);
		load = read_word(ifp);
		exec = read_word(ifp);
		attr = read_word(ifp);
		complen = read_word(ifp);
		info_word = read_word(ifp);

		/* Examine, and create nspark header */
		if (info_byte == AFS_DELETED)
		{
			free(header);
			free(header_ptr);
			continue;
		}
		/* BB changed next line for Borland C/C++ 4 */
		/* header_ptr->is_dir = (info_word >> 31); */
#ifdef __MSDOS__
		header_ptr->is_dir = (Halfword) (info_word >> 31);
#else
		header_ptr->is_dir = (info_word >> 31);
#endif
		header_ptr->info_byte = info_byte;
		header_ptr->info_word = info_word;
		/* BB changed next line for Borland C/C++ 4 */
		/* header_ptr->maxbits = (attr & 0xff00) >> 8; */
#ifdef __MSDOS__
		header_ptr->maxbits = (Halfword) (attr & 0xff00) >> 8;
#else
		header_ptr->maxbits = (attr & 0xff00) >> 8;
#endif
		/* BB changed constant in next line to long. */
		header_ptr->seek = (info_word & 0x7fffffffL) + data_start;
		header->comptype = info_byte;
		strcpy(header->name, (char *) name);
		header->complen = complen;
		header->date = 0;
		header->time = 0;
		header->crc = (Halfword) (attr >> 16);
		header->origlen = length;
		header->load = load;
		header->exec = exec;
		header->attr = attr & 0xff;

		arcfs_fixtime(header);

		if (info_byte == AFS_ENDDIR)
		{
			/* Just return comptype == 0 */
			*header = null_header;
			header_ptr->is_dir = 0;
			header_ptr->seek = 0;
		}

		/* If it is an ArcFS directory then convert to a Spark directory */
		if (header_ptr->is_dir)
		{
			/* Make sure filetype is DDC */
			header->comptype = CT_NOTCOMP2;
			/* BB changed constant in next line to long. */
			header->load = 0xfffddcffL;
		}

		/* Add list item to list */
		/* Doing it here ensures that deleted items are not added */
		header_ptr->header = header;
		if (header_list == NULL)
			header_list = header_ptr;
		else
			header_prev->next = header_ptr;
		header_prev = header_ptr;
#ifdef DEBUGGING
		print_header(header);
#endif
	}

	/* Return first element */
	header_ptr = header_list;
	header = header_ptr->header;
	/* Seek to start of data for first element */
	if ((!header_ptr->is_dir)
		&& (fseek(ifp, (long) header_ptr->seek, SEEK_SET)))
	{
		printf("Cannot seek compressed data in this file\n");
		return (&null_header);
	}
	/* Set up number of compression bits */
	arcfs_maxbits = header_ptr->maxbits;
	/*if (header_ptr->is_dir) header = &null_header; */
	header_ptr = header_ptr->next;
	return (header);
}
