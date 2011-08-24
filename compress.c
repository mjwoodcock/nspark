
/*
 * compress/uncompress archive
 *
 * Authors:	Spencer W. Thomas	(decvax!utah-cs!thomas)
 *		Jim McKie		(decvax!mcvax!jim)
 *		Steve Davies		(decvax!vax135!petsd!peora!srd)
 *		Ken Turkowski		(decvax!decwrl!turtlevax!ken)
 *		James A. Woods		(decvax!ihnp4!ames!jaw)
 *		Joe Orost		(decvax!vax135!petsd!joe)
 *
 * NOTE: these functions also support "squash" (which is just a
 * 13-bit compress), and "crunch" (which is a 12-bit compress
 * with additional run-length encoding).  AJD
 *
 * $Header: compress.c 1.11 95/08/01 $
 * $Log:	compress.c,v $
 * Revision 1.11 95/08/01  xx:xx:xx  BB
 * Quite a few changes for Borland C/C++
 * Made htab and codetab arrays dynamic.
 * (Compile with -DBB_HUGE_STATIC_ARRAYS if you DO want these
 * huge static arrays in your executable.)
 * Changed pointers to normalized or huge pointers because
 * arrays span more than 64k.
 * Changed a few types from int to long because 32bits integers
 * are needed.
 *
 * Revision 1.10 95/01/25  12:49:43  arb
 * Bug fixes caused by 1.9
 *
 * Revision 1.9  95/01/06  12:00:06  arb
 * Fixes for Alpha.
 *
 * Revision 1.8  94/02/28  23:57:55  arb
 * Fixed number of compression bits for ArcFS format archives
 *
 * Revision 1.7  93/08/20  11:35:20  arb
 * Prevent printing of "uncompressed" etc. if quiet flag is set
 *
 * Revision 1.6  92/12/07  17:17:28  duplain
 * reformatted source.
 *
 * Revision 1.5  92/11/09  14:48:00  duplain
 * Initialised offset and size from getcode() each time uncompress() called.
 *
 * Revision 1.4  92/11/02  11:43:14  duplain
 * Correct comment about crunch/squash in header.
 *
 * Revision 1.3  92/10/23  14:08:13  duplain
 * Minor changes to printf's at end of uncompress.
 *
 * Revision 1.2  92/10/01  11:20:19  duplain
 * Added check for EOF.
 *
 * Revision 1.1  92/09/29  18:02:14  duplain
 * Initial revision
 *
 */

#include <stdio.h>
#include "spark.h"
#include "pack.h"
#include "main.h"
#include "crc.h"
#include "garble.h"

/* BB changed next line because of conflict with Borland's io.h */

/* #include "io.h" */
#include "nsparkio.h"
#include "arcfs.h"

#ifdef __MSDOS__
#include <alloc.h>				/* for farcalloc() */
#include <stdlib.h>				/* for exit() */
#endif							/* __MSDOS__ */

#ifdef UNIX
static char rcsid[] = "$Header: compress.c 1.11 95/08/01 $";
#endif							/* UNIX */

#define PBITS 16
#define CRUNCHBITS 12
#define SQUASHBITS 13
#define COMPRESSBITS 16

/* BB changed constant in next line to long: 16bits 65536 == 0 ! */
#define HSIZE 65536L
#define INIT_BITS 9				/* initial number of bits/code */

/* BB changed next macros.
 * Arrays htab and codetab both exceed 64k. To prevent wraparound
   at the 64k boundary, normalized or huge pointers have to be used.
   Since subscripts are 16 bit ints under the Borland compiler,
   subscripts have to be made explicitely long.
   And finally COMPRESSBITS == 16, but 1 << 16 == 0 for 16 bits
   integers! */

/* #define MAXCODE(n_bits)	((1 << (n_bits)) - 1) */

/* #define htabof(i) htab[i] */

/* #define codetabof(i) codetab[i] */

/* #define tab_prefixof(i)	codetabof(i) */

/* #define tab_suffixof(i)	((char_type *)(htab))[i] */

/* #define de_stack ((char_type *)&tab_suffixof(1<<COMPRESSBITS)) */
#ifdef __MSDOS__
#define MAXCODE(n_bits)	((long)(1L << (n_bits)) - 1L)
#define htabof(i) htab[(long)(i)]
#define codetabof(i) codetab[(long)(i)]
#define tab_prefixof(i)	codetabof(i)
#define tab_suffixof(i)	((char_type huge *)(htab))[(long)(i)]
#define de_stack \
          ((char_type huge *)&tab_suffixof(1L<<COMPRESSBITS))
#else
#define MAXCODE(n_bits)	((1 << (n_bits)) - 1)
#define htabof(i) htab[i]
#define codetabof(i) codetab[i]
#define tab_prefixof(i)	codetabof(i)
#define tab_suffixof(i)	((char_type *)(htab))[i]
#define de_stack ((char_type *)&tab_suffixof(1<<COMPRESSBITS))
#endif							/* __MSDOS__ */
#define FIRST 257				/* first free entry */
#define	CLEAR 256				/* table clear output code */

/* BB changed next two lines. For 16 bits, the maximum code_int
   becomes zero again! (1 << 16 == 0).
   Debugging the un*x version shows that
   count_int should be a 32bits integer! */

/* typedef int code_int; */

/* typedef int count_int; */
#ifdef __MSDOS__
typedef long code_int;
typedef long count_int;
#else
typedef int count_int;
typedef int code_int;
#endif							/* __MSDOS__ */
typedef unsigned char char_type;

static int n_bits;				/* number of bits/code */
static int maxbits;				/* user settable max # bits/code */
static code_int maxcode;		/* maximum code, given n_bits */
static code_int maxmaxcode;		/* should NEVER generate this code */

/* BB changed next two lines.
   Under Borland C/C++ static arrays are REALLY static, i.e. they
   clog the executable with some 384k of `empty space'. So use
   dynamic arrays instead. */

/* static count_int htab[HSIZE]; */

/* static unsigned short codetab[HSIZE]; */
#ifdef __MSDOS__

/* For those that do want to use static arrays:
   define BB_HUGE_STATIC_ARRAYS. */
#ifdef BB_HUGE_STATIC_ARRAYS
static count_int huge htab[HSIZE];
static unsigned short huge codetab[HSIZE];
#else							/* BB_HUGE_STATIC_ARRAYS */
static count_int huge *htab = NULL;
static unsigned short huge *codetab = NULL;
#endif							/* BB_HUGE_STATIC_ARRAYS */
#else							/* __MSDOS__ */
static count_int htab[HSIZE];
static unsigned short codetab[HSIZE];
#endif							/* __MSDOS__ */
static char_type rmask[9] =
	{ 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };
static code_int free_ent;		/* first unused entry */
static int clear_flg;
static long readsize;			/* number of bytes left to read */

/* BB changed next line. readsize and other ``ints'' are (32bits) longs
   so the next two should be longs too. Or rather size_t because that
   is the return type of fread(). */

								/* static int offset, size; *//* from getcode() */
#ifdef __MSDOS__
static size_t offset, size;		/* from getcode() */
#else
static int offset, size;		/* from getcode() */
#endif							/* __MSDOS__ */

static code_int getcode P__((FILE * ifp));


Status
uncompress(header, ifp, ofp, type)
	Header *header;
	FILE *ifp, *ofp;
	CompType type;
{
	/* BB changed next line. stackp points to huge pointers. */
	/* register char_type *stackp; */
#ifdef __MSDOS__
	register char_type huge *stackp;
#else
	register char_type *stackp;
#endif							/* __MSDOS__ */
	/* BB changed next line. Since code_ints should be longs on
	   16bits machines and this int gets assigned from and to
	   code_ints, it should be a long as well. */
	/* register int finchar; */
#ifdef __MSDOS__
	register long finchar;
#else
	register int finchar;
#endif							/* __MSDOS__ */
	register code_int code, oldcode, incode;
	char *message;

	init_garble();

#if defined(__MSDOS__) && !defined(BB_HUGE_STATIC_ARRAYS)
	if (!htab)
		htab = (count_int huge *) farcalloc(HSIZE, sizeof(count_int));
	if (!codetab)
		codetab =
			(unsigned short huge *) farcalloc(HSIZE,
											  sizeof(unsigned short));
	if (!htab || !codetab)
	{
		fprintf(stderr, "%s: uncompress: out of memory\n", ourname);
		exit(1);
	}
#endif							/* __MSDOS__ && ! BB_HUGE_STATIC_ARRAYS */

	crc = 0;
	clear_flg = 0;
	offset = 0;
	size = 0;
	readsize = header->complen;

	if (type == SQUASH)
		maxbits = SQUASHBITS;
	else if (type == UNIX_COMPRESS)
	{
		/* Read the unix compress header */
		read_byte(ifp);
		read_byte(ifp);
		maxbits = read_byte(ifp) & 0x1f;
		readsize -= 3;
	}
	else
	{
		if (arcfs)
		{
			maxbits = arcfs_maxbits;
			ungarble('\0'); // Need to consume one byte of password.
		}
		else
		{
			maxbits = read_byte(ifp);
			readsize--;
		}
	}
	maxmaxcode = 1 << maxbits;

	/*
	 * As above, initialize the first 256 entries in the table.
	 */
	maxcode = MAXCODE(n_bits = INIT_BITS);
	for (code = 255; code >= 0; code--)
	{
		tab_prefixof(code) = 0;
		tab_suffixof(code) = (char_type) code;
	}
	free_ent = FIRST;

	finchar = oldcode = getcode(ifp);
	if (oldcode == -1)			/* EOF already? */
		goto compress_exit;		/* Get out of here */

	/* first code must be 8 bits = char */
	if (type == CRUNCH)
	{
		putc_init();
		/* BB changed next line for Borland C/C++ 4 */
		/* putc_ncr(ofp, finchar); */
#ifdef __MSDOS__
		putc_ncr(ofp, (Byte) finchar);
#else
		putc_ncr(ofp, finchar);
#endif							/* __MSDOS__ */
	}
	else
	{
		/* BB changed next three lines for Borland C/C++ 4 */
		/* if (!testing) */
		/*     write_byte(ofp, finchar); */
		/* calccrc(finchar); */
		/* (Could be neater) */
#ifdef __MSDOS__
		if (!testing)
			write_byte(ofp, (Byte) finchar);
		calccrc((Byte) finchar);
#else
		if (!testing)
			write_byte(ofp, finchar);
		calccrc(finchar);
#endif							/* __MSDOS__ */
	}

	stackp = de_stack;

	while ((code = getcode(ifp)) != -1)
	{
		if (check_stream(ifp) != FNOERR)
			break;
		if (code == CLEAR)
		{
			for (code = 255; code >= 0; code--)
				tab_prefixof(code) = 0;
			clear_flg = 1;
			free_ent = FIRST - 1;
			if ((code = getcode(ifp)) == -1)	/* O, untimely death! */
				break;
		}
		incode = code;
		/*
		 * Special case for KwKwK string.
		 */
		if (code >= free_ent)
		{
			/* BB changed next line for Borland C/C++ 4 */
			/* *stackp++ = finchar; */
#ifdef __MSDOS__
			*stackp++ = (char_type) finchar;
#else
			*stackp++ = finchar;
#endif
			code = oldcode;
		}
		/*
		 * Generate output characters in reverse order
		 */

		while (code >= 256)
		{
			if ((char *)(stackp+1) > (char *)(&htab[0] + HSIZE))
			{
				fprintf(stderr, "%s: uncompress: corrupt or garbled archive file\n", ourname);
				exit(1);
			}
			*stackp++ = tab_suffixof(code);
			code = tab_prefixof(code);
		}
		/* BB changed next line for Borland C/C++ 4 */
		/* *stackp++ = finchar = tab_suffixof(code); */
#ifdef __MSDOS__
		finchar = tab_suffixof(code);
		*stackp++ = (char_type) finchar;
#else
		if ((char *)(stackp+1) > (char *)(&htab[0] + HSIZE))
		{
			fprintf(stderr, "%s: uncompress: corrupt or garbled archive file\n", ourname);
			exit(1);
		}
		*stackp++ = finchar = tab_suffixof(code);
#endif

		/*
		 * And put them out in forward order
		 */
		while (stackp > de_stack)
		{
			stackp--;
			if (type == CRUNCH)
				putc_ncr(ofp, *stackp);
			else
			{
				if (!testing)
					write_byte(ofp, *stackp);
				calccrc(*stackp);
			}
		}

		/*
		 * Generate the new entry.
		 */
		if ((code = free_ent) < maxmaxcode)
		{
			/* BB changed next two lines for Borland C/C++ 4 */
			/* tab_prefixof(code) = oldcode; */
			/* tab_suffixof(code) = finchar; */
#ifdef __MSDOS__
			tab_prefixof(code) = (unsigned short) oldcode;
			tab_suffixof(code) = (char_type) finchar;
#else
			tab_prefixof(code) = oldcode;
			tab_suffixof(code) = finchar;
#endif							/* __MSDOS__ */
			free_ent = code + 1;
		}
		/*
		 * Remember previous code.
		 */
		oldcode = incode;
	}
  compress_exit:
	if (check_stream(ifp) == FRWERR)
		return (RERR);
	if (!testing && check_stream(ofp) == FRWERR)
		return (WERR);
	if ((Halfword) crc != header->crc)
		return (CRCERR);
	if (testing)
		switch (type)
		{
		case COMPRESS:
			message = "OK (compressed)";
			break;
		case CRUNCH:
			message = "OK (crunched)";
			break;
		case SQUASH:
			message = "OK (squashed)";
			break;
		default:
			message = "internal error";
			break;
		}
	else
		switch (type)
		{
		case COMPRESS:
			message = "uncompressed";
			break;
		case CRUNCH:
			message = "uncrunched";
			break;
		case SQUASH:
			message = "unsquashed";
			break;
		default:
			message = "internal error";
			break;
		}
	if (!quiet)
		printf(message);
	return (NOERR);
}

/*
 * Read one code from the input.  If EOF, return -1.
 */
static code_int
getcode(ifp)
	FILE *ifp;
{
	register code_int code;
	static char_type buf[COMPRESSBITS];
	register int r_off, bits;
	int i;
	/* BB changed next line. We are doing pointer-artithmatics
	   and that can be dangerous if other than normalized (huge)
	   pointers are being used. */
	/* register char_type *bp = buf; */
#ifdef __MSDOS__
	register char_type huge *bp = buf;
#else
	register char_type *bp = buf;
#endif							/* __MSDOS__ */

	if (clear_flg > 0 || offset >= size || free_ent > maxcode)
	{
		/*
		 * If the next entry will be too big for the current code
		 * size, then we must increase the size.  This implies
		 * reading a new buffer full, too.
		 */
		if (free_ent > maxcode)
		{
			n_bits++;
			maxcode = n_bits == maxbits ? maxmaxcode : MAXCODE(n_bits);
		}
		if (clear_flg > 0)
		{
			maxcode = MAXCODE(n_bits = INIT_BITS);
			clear_flg = 0;
		}
		if (readsize == 0)
			return (-1);
		/* BB added cast to next line */
		/* size = readsize < n_bits ? readsize : n_bits; */
#ifdef __MSDOS__
		size = readsize < n_bits ? (size_t) readsize : n_bits;
#else
		size = readsize < n_bits ? readsize : n_bits;
#endif							/* __MSDOS__ */
		size = fread(buf, 1, size, ifp);
		if (size <= 0)
			return (-1);		/* end of file */
		for (i = 0; i < size; i++)
		{
			buf[i] = ungarble(buf[i]);
		}
		readsize -= size;
		offset = 0;
		/* Round size down to integral number of codes */
		size = (size << 3) - (n_bits - 1);
	}
	r_off = offset;
	bits = n_bits;

	/*
	 * Get to the first byte.
	 */
	bp += (r_off >> 3);
	r_off &= 7;
	/* Get first part (low order bits) */

	code = (*bp++ >> r_off);
	bits -= (8 - r_off);
	r_off = 8 - r_off;			/* now, offset into code word */
	/* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */
	if (bits >= 8)
	{
		code |= *bp++ << r_off;
		r_off += 8;
		bits -= 8;
	}
	/* high order bits. */
	code |= (*bp & rmask[bits]) << r_off;
	offset += n_bits;

	return (code);
}
