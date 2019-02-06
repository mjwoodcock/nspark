
/*
 * file input/output
 *
 * $Header: io.c 1.14 95/08/01 $
 * $Log:	io.c,v $
 * Revision 1.14  95/08/01  xx:xx:xx  BB
 * Fixed for Borland C/C++
 *
 * Revision 1.13  94/12/12  17:30:45  arb
 * Added code for ArcFS outputsize checking
 *
 * Revision 1.12  93/08/20  12:31:20  arb
 * Added code for ArcFS archive headers
 *
 * Revision 1.11  93/08/20  10:30:50  arb
 * Added code for -C option to convert filenames to lowercase
 *
 * Revision 1.10  93/08/20  10:30:51  arb
 * Changed read_header() to allow top-bit-set characters in RISCOS filenames
 *
 * Revision 1.9  93/03/05  14:44:02  arb
 * Added <string.h> for RISCOS, needed for memset
 *
 * Revision 1.8  92/12/09  11:40:30  duplain
 * Changed ret in check_stream() from type int.  #ifdef'd out write_halfword()
 * and write_word().
 *
 * Revision 1.7  92/12/07  17:18:25  duplain
 * reformatted source.
 *
 * Revision 1.6  92/11/04  16:57:49  duplain
 * Changed read_header() so it doesn't read the load/exec/attr fields if
 * PC archive file.
 *
 * Revision 1.5  92/10/07  10:36:56  duplain
 * Changed order of function so no need to include "io.h" (gcc
 * complained on some platforms).
 *
 * Revision 1.4  92/10/05  11:01:07  duplain
 * Recoded write_word() and write_halfword(). read_header() now clears the
 * header structure prior to reading data into it.
 *
 * Revision 1.3  92/10/02  17:41:49  duplain
 * Fixed read_header() so it returns immediately if comptype & 0x7f == 0.
 *
 * Revision 1.2  92/10/01  11:20:32  duplain
 * Moved reading of STARTBYTE to unarc.c .
 *
 * Revision 1.1  92/09/29  18:02:19  duplain
 * Initial revision
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "spark.h"
#include "main.h"
#include "error.h"
#include "arcfs.h"

/* BB changed next line */

/* #ifdef RISCOS */
#if defined(RISCOS) || defined(__MSDOS__)
#include <string.h>				/* for memset */

/* BB added next two lines */
#endif							/* RISCOS || __MSDOS__ */
#ifdef RISCOS					/* next line RISC OS only */
#define isascii(x) 1
#endif							/* RISCOS */

#include "nsparkio.h"


/*
 * check for EOF or write/read errors on stream.
 */
Ferror
check_stream(FILE *fp)
{
	Ferror ret = FNOERR;

	if (feof(fp))
		ret = FEND;
	else if (ferror(fp))
		ret = FRWERR;
	if (ret != FNOERR)
		clearerr(fp);
	return (ret);
}

/*
 * read a byte from the input stream.
 */
Byte
read_byte(FILE *ifp)
{
	return ((Byte) getc(ifp));
}

/*
 * read a little-endian 2-byte halfword from the input stream.
 */
Halfword
read_halfword(FILE *ifp)
{
	union
	{
		Halfword h;
		Byte b[sizeof(Halfword)];
	}
	ret;

	if (fread((char *) &ret.h, 1, sizeof(Halfword), ifp)!=sizeof(Halfword)) {
		error("Read error!");
	}
#if defined(__MSDOS__)
	/* MSDOS reads bytes in the correct order.  Save a bit of time
	 * by returning the value */
	return (ret.h);
#else
	/* Ensure the data is in the correct order */
	return (ret.b[0] | ret.b[1] << 8);
#endif
}

/*
 * read a little-endian 4-byte word from the input stream.
 */
Word
read_word(FILE *ifp)
{
	union
	{
		Word w;
		Byte b[sizeof(Word)];
	}
	ret;

	if (fread((char *) &ret.w, 1, sizeof(Word), ifp)!=sizeof(Word)) {
		error("Read error!");
	}
#if defined(__MSDOS__)
	/* MSDOS reads bytes in the correct order.  Save a bit of time
	 * by returning the value */
	return (ret.w);
#else
	/* Ensure the data is in the correct order */
	return (ret.b[0] | (ret.b[1] << 8) | (ret.b[2] << 16) | (ret.b[3] << 24));
#endif
}

/*
 * write a byte to the output stream.
 */
void
write_byte(FILE *ofp, Byte byte)
{
	if (writesize-- > 0)
		putc((int) byte, ofp);
}

#ifdef notyet

/*
 * write a little-endian 2-byte halfword to the output stream.
 */
void
write_halfword(FILE *ofp, Halfword halfword)
{
	write_byte(ofp, halfword & 0xff);
	write_byte(ofp, (halfword >> 8) & 0xff);
}

/*
 * write a little-endian 4-byte word to the output stream.
 */
void
write_word(FILE *ofp, Word word)
{
	write_byte(ofp, word & 0xff);
	write_byte(ofp, (word >> 8) & 0xff);
	write_byte(ofp, (word >> 16) & 0xff);
	write_byte(ofp, (word >> 24) & 0xff);
}

#endif							/* notyet */

/*
 * read a compression-header from the file
 */
Header *
read_header(FILE *ifp)
{
	static Header header;
	register int i;
	register char *cptr;
	Byte byte;

	memset((char *) &header, '\0', sizeof(header));

	if (arcfs)
		return (arcfs_read_header(ifp));

	header.comptype = read_byte(ifp);
	if (!(header.comptype & ~ARCHPACK))
		return (&header);		/* EOF */

	for (i = 0, cptr = header.name; i <= 12; i++, cptr++)
	{
		byte = read_byte(ifp);
#ifdef RISCOS
		if (byte < ' ')
#else
		if (byte < ' ' || byte > '~')
#endif
			byte = '\0';
		else if (byte == PATHSEP)	/* illegal in filename */
			byte = '_';
		*cptr = byte;
	}
	*cptr = '\0';				/* null terminate */
	if (singlecase)
	{
		for (i = 0, cptr = header.name; i <= 12; i++, cptr++)
			if (isascii(*cptr) && isupper(*cptr))
				*cptr = tolower(*cptr);
	}

	header.complen = read_word(ifp);
	header.date = read_halfword(ifp);
	header.time = read_halfword(ifp);
	header.crc = read_halfword(ifp);
	if ((header.comptype & ~ARCHPACK) > CT_NOTCOMP)
		header.origlen = read_word(ifp);
	else
		header.origlen = header.complen;
	if (header.comptype & ARCHPACK)
	{
		header.load = read_word(ifp);
		header.exec = read_word(ifp);
		header.attr = read_word(ifp);
	}

	if (check_stream(ifp) == FRWERR)
		return (NULL);
	return (&header);
}

Status
read_sqsh_header(FILE *fp, SqshHeader *header)
{
	if (fread(header->magic, 1, 4, fp) != 4)
	{
		return RERR;
	}
	if (strncmp(header->magic, "SQSH", 4) != 0)
	{
		error("Not a RISC OS squash file");
		return RERR;
	}
	header->origlen = read_word(fp);
	header->load = read_word(fp);
	header->exec = read_word(fp);
	header->reserved = read_word(fp);

	return NOERR;
}

void
sqsh_header_to_header(SqshHeader *sqsh_header, Header *header)
{
	header->load = sqsh_header->load;
	header->exec = sqsh_header->exec;
	header->origlen = sqsh_header->origlen;
	arcfs_fixtime(header);
}
