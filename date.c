
/*
 * manipulate date types
 *
 * $Header: date.c 1.4 92/12/07 $
 * $Log:	date.c,v $
 * Revision 1.4  92/12/07  17:17:47  duplain
 * reformatted source.
 * 
 * Revision 1.3  92/11/06  12:43:06  duplain
 * Removed check for "time-stamped header" in makedate().
 * 
 * Revision 1.2  92/10/06  12:12:10  duplain
 * Fixed makedate().
 * 
 * Revision 1.1  92/09/29  18:02:17  duplain
 * Initial revision
 * 
 */

#include <stdio.h>
#include "spark.h"

static char *monthnames[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

/*
 * Build date structure from load and exec fields
 */
Date *
makedate(Header *header)
{
	static Date date;
	register Halfword arcdate = header->date;
	register Halfword arctime = header->time;

	date.second = (arctime & 0x1f) * 2;
	date.minute = (arctime >> 5) & 0x3f;
	date.hour = (arctime >> 11) & 0x1f;
	date.day = arcdate & 0x1f;
	date.month = (arcdate >> 5) & 0x0f;
	date.year = ((arcdate >> 9) & 0x7f) + 80;
	return (&date);
}

char *
monthname(int month)
{
	if (month > 0 && month < 13)
		return (monthnames[month - 1]);
	else
		return ("XXX");
}
