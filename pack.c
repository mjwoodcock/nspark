
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

/* BB changed next line because of conflict with Borland's io.h */

/* #include "io.h" */
#include "nsparkio.h"

/* BB added next include for prototypes for Borland C/C++ */
#ifdef __MSDOS__
#include "pack.h"
#endif							/* __MSDOS__ */

#ifdef UNIX
static char rcsid[] = "$Header: pack.c 1.5 95/08/01 $";
#endif							/* UNIX */

static short running;


void
putc_init()
{
	running = 0;
}

/*
 * write run-length encoding to output file
 */

void
putc_ncr(ofp, byte)
	FILE *ofp;
	Byte byte;
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
unpack(header, ifp, ofp)
	Header *header;
	FILE *ifp, *ofp;
{
	/* BB changed next line: complen is a long */
	/* register len = header->complen; */
#ifdef __MSDOS__
	register long len = header->complen;
#else
	register len = header->complen;
#endif							/* __MSDOS__ */

	crc = 0;
	putc_init();
	while (len--)
	{
		if (check_stream(ifp) != FNOERR)
			break;
		putc_ncr(ofp, read_byte(ifp));
	}

	if (check_stream(ifp) == FRWERR)
		return (RERR);
	if (!testing && check_stream(ofp) == FRWERR)
		return (WERR);
	if ((Halfword) crc != header->crc)
		return (CRCERR);
	if (testing)
		printf("OK (packed)");
	else
		printf("unpacked");
	return (NOERR);
}
