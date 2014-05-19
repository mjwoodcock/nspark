
/*
 * Operating System specific function (DOS)
 *
 * $Header: msdos.c 1.0 95/07/21 $
 * $Log:	msdos.c,v $
 * Revision 1.0  95/07/21  xx:xx:xx  BB
 * Initial revision, based on winnt.c.
 *
 */

#include "spark.h"
#include "date.h"
#include <sys\stat.h>
#include <dir.h>
#include <time.h>
#include <utime.h>
#include <string.h>				/* for memset */
#include "os.h"

/*
 * return the length of a file
 */
Word
filesize(char *pathname)
{
	struct stat statb;

	if (stat(pathname, &statb) < 0)
		return 0;

	return (Word) statb.st_size;;
}

/*
 * test for the existance of a file or directory
 */
Ftype
exist(char *pathname)
{
	struct stat statb;

	if (stat(pathname, &statb) < 0)
		return NOEXIST;

	if (statb.st_mode & S_IFDIR)
		return ISDIR;

	return (ISFILE);
}

/*
 * make a directory
 */
int
makedir(char *pathname)
{
	return mkdir(pathname);
}

/*
 * stamp a file with date and time
 */
int
filestamp(Header *header, char *filename)
{
	Date *date;
	struct tm tm;
	struct utimbuf utimbuf;
	time_t filetime;

	/* BB: DOS utime() cannot stamp directories.
	   It could be done by directly editing the directory entries
	   (simulate a disc editor) but since e.g. pkunzip does not
	   stamp extracted directories either, I did not bother and
	   kept the next line from winnt.c.
	   ``It is left as an exercice to the interested reader.''
	   NB: Do not forget to stamp the . entry in the directory
	   itself! */
	if (exist(filename) == ISDIR)
		return (0);

	if ((header->load & (Word) 0xfff00000UL) != (Word) 0xfff00000UL)
		return (0);				/* not a timestamp */

	memset((char *) &tm, '\0', sizeof(tm));

	/* Borland C/C++ 4 is `a bit fuzzy' about the next line 8-( */
	/* if (!(date = makedate(header))) */
	if ((date = makedate(header)) == 0)
		return (-1);

	tm.tm_sec = date->second;
	tm.tm_min = date->minute;
	tm.tm_hour = date->hour;
	tm.tm_mday = date->day;
	tm.tm_mon = date->month - 1;
	tm.tm_year = date->year;
	filetime = mktime(&tm);

	utimbuf.actime = filetime;
	utimbuf.modtime = filetime;
	return (utime(filename, &utimbuf));
}
