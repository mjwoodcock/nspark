
/*
 * store/unstore archive file
 *
 * $Header: store.c 1.5 95/08/01 $
 * $Log:	store.c,v $
 * Revision 1.5  95/08/01  xx:xx:xx  BB
 * Fixed for Borland C/C++
 *
 * Revision 1.4  93/08/20  11:50:20  arb
 * Do not print "unstored" if in quiet mode
 *
 * Revision 1.3  92/12/07  17:19:39  duplain
 * reformatted source.
 *
 * Revision 1.2  92/10/01  11:22:46  duplain
 * Added check for EOF.
 *
 * Revision 1.1  92/09/29  18:02:26  duplain
 * Initial revision
 *
 */

#include <stdio.h>
#include "spark.h"
#include "main.h"
#include "crc.h"
#include "garble.h"

/* BB changed next line because of conflict with Borland's io.h */

/* #include "io.h" */
#include "nsparkio.h"

#ifdef UNIX
static char rcsid[] = "$Header: store.c 1.5 95/08/01 $";
#endif							/* UNIX */

Status
unstore(header, ifp, ofp)
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
	register Byte byte;

    init_garble();

	crc = 0;
	while (len--)
	{
		if (check_stream(ifp) != FNOERR)
			break;
		byte = read_byte(ifp);
		byte = ungarble(byte);
		calccrc(byte);
		if (!testing)
			write_byte(ofp, byte);
	}

	if (check_stream(ifp) == FRWERR)
		return (RERR);
	if (!testing && check_stream(ofp) == FRWERR)
		return (WERR);
	if ((Halfword) crc != header->crc)
		return (CRCERR);
	if (testing)
	{
		if (!quiet)
			printf("OK (stored)");
	}
	else if (!quiet)
		printf("unstored");
	return (NOERR);
}
