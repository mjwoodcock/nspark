
/*
 * pack/unpack archive files
 *
 * $Header: pack.c 1.5 95/08/01 $
 * $Log:	pack.c,v $
 * Revision 1.5  95/08/01  xx:xx:xx  BB
 * Fixed for Borland C/C++
 *
 * Revision 1.4  92/12/07  17:19:29  duplain
 * reformatted source.
 *
 * Revision 1.3  92/11/09  14:48:36  duplain
 * Added putc_init() to re-initialise variables.
 *
 * Revision 1.2  92/10/01  11:22:35  duplain
 * Added check for EOF.
 *
 * Revision 1.1  92/09/29  18:02:25  duplain
 * Initial revision
 *
 */

#include <stdio.h>
#include "spark.h"
#include "main.h"
#include "crc.h"
#include "garble.h"
#include "error.h"

/* BB changed next line because of conflict with Borland's io.h */

/* #include "io.h" */
#include "nsparkio.h"

#include "pack.h"

static short running;
static Word complen;


void
putc_init()
{
	running = 0;
}

/*
 * write run-length encoding to output file
 */

void
putc_ncr(FILE *ofp, Byte byte)
{
	static Byte prevbyte;

	if (running)
	{
		if (!byte)
		{						/* means write RUNMARK to output */
			calccrc(RUNMARK);
			if (!testing)
				write_byte(ofp, RUNMARK);
		}
		else
		{
			/* BB changed next line */
			/* Borland C++ is `a bit fuzzy' about next line */
			/*  while (--byte) { */
#ifdef __MSDOS__
			while (--byte != 0)
			{
#else
			while (--byte)
			{
#endif
				calccrc(prevbyte);
				if (!testing)
					write_byte(ofp, prevbyte);
			}
		}
		running = 0;
	}
	else if (byte == RUNMARK)
	{
		running++;
	}
	else
	{
		prevbyte = byte;		/* save in case next byte is RUNMARK */
		calccrc(byte);
		if (!testing)
			write_byte(ofp, byte);
	}
}

Status
unpack(Header *header, FILE *ifp, FILE *ofp)
{
	register Word len = header->complen;

    init_garble();

	crc = 0;
	putc_init();
	while (len--)
	{
		if (check_stream(ifp) != FNOERR)
			break;
		putc_ncr(ofp, ungarble(read_byte(ifp)));
	}

	if (check_stream(ifp) == FRWERR)
		return (RERR);
	if (!testing && check_stream(ofp) == FRWERR)
		return (WERR);
	if ((Halfword) crc != header->crc)
		return (CRCERR);
	if (testing)
		msg("OK (packed)");
	else
		msg("unpacked");
	return (NOERR);
}

void
write_ncr(FILE *ofp, Byte byte, int bytecount)
{
	int i;

	if (bytecount > 1)
	{
		fputc((int)garble(byte), ofp);
		fputc((int)garble(RUNMARK), ofp);
		fputc((int)garble(bytecount), ofp);
		complen += 3;
		for (i = 0; i < bytecount; i++)
		{
			calccrc(byte);
		}
	}
	else
	{
		if (byte == RUNMARK)
		{
			calccrc(RUNMARK);
			fputc((int)garble(RUNMARK), ofp);
			fputc(garble(0), ofp);
			complen += 2;
		}
		else
		{
			calccrc(byte);
			fputc((int)garble(byte), ofp);
			complen += 1;
		}
	}
}

Status
pack(Header *header, FILE *ifp, FILE *ofp)
{
	register Word len = header->origlen;
	Byte prevbyte = '\0', byte;
	int bytecount = 0;

    init_garble();

	complen = 0;
	crc = 0;
	prevbyte = read_byte(ifp);
	len--;
	bytecount = 1;
	while (len--)
	{
		byte = read_byte(ifp);
		if(prevbyte == RUNMARK)
		{
			write_ncr(ofp, prevbyte, 1);
			bytecount = 1;
		}
		else if (byte == prevbyte && bytecount < 254)
		{
			bytecount++;
		}
		else
		{
			write_ncr(ofp, prevbyte, bytecount);
			bytecount = 1;
		}
		prevbyte = byte;
		if (check_stream(ifp) != FNOERR)
			break;
	}
	write_ncr(ofp, prevbyte, bytecount);

	if (check_stream(ifp) == FRWERR)
		return (RERR);
	if (!testing && check_stream(ofp) == FRWERR)
		return (WERR);
	if (testing)
		msg("OK (packed)");
	else
		msg("packed");

	header->crc = (Halfword) crc;
	header->complen = complen;

	return (NOERR);
}
