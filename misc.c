
/*
 * miscellaneous functions
 *
 * Revision 1.14 99/03/17 MU
 * Also capitilised the hexadecimal output strings.
 *
 * $Header: misc.c 1.12 95/08/01 $
 * $Log:	misc.c,v $
 * Revision 1.13  95/08/01  xx:xx:xx  BB
 * Fixed for Borland C/C++
 * Made inlist() case insensitive (DOS only) because DOS filenames are case
 * insensitive.
 * Made basename safe for null-strings.
 *
 * Revision 1.12  92/12/22  09:54:29  duplain
 * Changed #include <malloc.h> to #include <stdlib.h> .
 *
 * Revision 1.11  92/12/09  09:42:36  duplain
 * Simplified append_type().  Changed append_type() to append lowercase types.
 *
 * Revision 1.10  92/12/08  10:20:07  duplain
 * Added append_type().
 *
 * Revision 1.9  92/12/07  17:18:58  duplain
 * reformatted source.
 *
 * Revision 1.8  92/11/12  09:03:30  duplain
 * Fixed bug with realloc() size in uplevel().
 *
 * Revision 1.7  92/11/06  12:42:28  duplain
 * Changed print_details() so it supports PC archive headers correctly.
 *
 * Revision 1.6  92/11/04  16:56:14  duplain
 * Added check for PC archive header in print_header().
 *
 * Revision 1.5  92/10/09  18:06:58  duplain
 * Added "+1" to malloc() call in riscos_path()... SCO UNIX was quiet right
 * to core dump :-)
 *
 * Revision 1.4  92/10/07  10:56:39  duplain
 * Added check for SYSV2 when including <malloc.h>.  Made riscos_path() compile
 * for non-RISCOS systems only.
 *
 * Revision 1.3  92/10/06  12:12:29  duplain
 * Removed reference to date->csecond in print_details().
 *
 * Revision 1.2  92/09/30  10:26:58  duplain
 * Fixed basename().  Added riscos_path().
 *
 * Revision 1.1  92/09/29  18:02:21  duplain
 * Initial revision
 *
 */

#ifdef BSD42
#include <strings.h>
#else							/* not 4.2BSD */
#include <stdlib.h>
#include <string.h>
#endif							/* 4.2BSD */

#include <stdio.h>
#include "spark.h"
#include "main.h"

#if defined(BSD42) || defined(SYSV2)
extern char *malloc(unsigned len);
extern char *realloc(char *ptr, unsigned len);
#endif							/* 4.2BSD or SysV.2 */

#include "date.h"
#include "misc.h"

/*
 * return last element in pathname
 */
char *
basename(char *s)
{
	/* BB changed next line to cope with null-pointers.
	   (IRIX's strlen() produces a coredump when s == NULL). */
	/* char *cptr = s + strlen(s); */
	char *cptr;
	if (!s)
		return NULL;
	cptr = s + strlen(s);
	while (cptr > s)
	{
		if (*cptr == PATHSEP)
			return (++cptr);
		cptr--;
	}
	return (s);
}

#if defined(RISCOS)
#define DOTARC	"_arc"
#define DOT	'_'
#else							/* not RISCOS */
#define DOTARC	".arc"
#define DOT	'.'
#endif							/* RISCOS */

/*
 * append ".arc" ("_arc" in RISCOS) to a string, if an extension doesn't
 * already exist, and return the new string.
 */
char *
name_dot_arc(char *s)
{
	static char *newname = NULL;

	/*
	 * check that there's room for the extension
	 */
	if (strlen(basename(s)) + sizeof(DOTARC) - 1 > FILENAMELEN)
		return (s);

	/*
	 * free previous allocation (hope it's finished with :-/)
	 */
	if (newname)
		free(newname);
	newname = malloc(strlen(s) + sizeof(DOTARC));
	if (!newname)
		return (s);				/* don't complain */
	strcpy(newname, s);
	strcat(newname, DOTARC);
	return (newname);
}

/*
 * turn a local-host pathname into a RISC OS path
 */

#ifndef RISCOS
char *
riscos_path(register char *s)
{
	static char *riscosname = NULL;
	register char *cptr;

	if (riscosname)
		free(riscosname);

	riscosname = malloc(strlen(s) + 1);
	if (!riscosname)
		return (NULL);
	for (cptr = riscosname; *s; s++, cptr++)
		if (*s == PATHSEP)
			*cptr = '.';
		else
			*cptr = *s;
	*cptr = '\0';
	return (riscosname);
}
#endif							/* RISC OS */

static char *pathname = NULL;

/*
 * uplevel() and downlevel() maintain the pathname as directories are found
 * within the archive
 */
char *
uplevel()
{
	register char *cptr;
	register olen, nlen;

	if (!pathname)
		return (NULL);

	olen = strlen(pathname);
	cptr = pathname + olen - 1;
	while (cptr > pathname)
		if (*cptr == PATHSEP)
		{
			*cptr = '\0';
			break;
		}
		else
			cptr--;

	if (cptr == pathname)
	{
		free(pathname);
		pathname = NULL;
	}
	else
	{
		nlen = strlen(pathname);
		if (nlen < olen)
			pathname = realloc(pathname, nlen + 1);
	}
	return (pathname);
}

char *
downlevel(char *filename)
{
	register len, flen;

	if (!pathname)
		len = 0;
	else
		len = strlen(pathname);

	flen = strlen(filename);
	if (!len)
	{
		pathname = malloc(flen + 1);
		if (pathname)
			strcpy(pathname, filename);
	}
	else
	{
		pathname = realloc(pathname, len + flen + 2);
		if (pathname)
		{
			strcat(pathname, PATHSEPSTR);
			strcat(pathname, filename);
		}
	}
	return (pathname);
}

char *
get_comp_desc(Byte comptype)
{
	switch (comptype & ~ARCHPACK)
	{
	case CT_NOTCOMP:
	case CT_NOTCOMP2:
		return "Stored";
	case CT_PACK:
		return "Packed";
	case CT_CRUNCH:
		return "Crunched";
	case CT_SQUASH:
		return "Squashed";
	case CT_COMP:
		return "Compressed";
	default:
		return "Unknown";
	}

	return "Unknown";
}

/*
 * print archive file details (size, data and time)
 */
void
print_details(Header *header)
{
	Date *date;

	if (!header)
		return;

	if (header->comptype & ARCHPACK)
	{
		/* Archimedes archive header */

		/* BB changed constants in next line to long */
		if ((header->load & (Word) 0xfff00000l) == (Word) 0xfff00000l)
		{
			/* time stamp valid */
			date = makedate(header);
			printf("%8ld %02d-%s-%04d %02d:%02d:%02d  &%03X %s",
				   header->origlen, date->day,
				   monthname(date->month), date->year + 1900,
				   date->hour, date->minute, date->second,
				   (header->load >> 8) & 0xfff,
				   get_comp_desc(header->comptype));

		}
		else
		{
			/* load/exec only */
			/* BB added long to first format in next line.
			   Header.origlen is a Word (i.e. a long) */
#ifdef __MSDOS__
			printf("%8ld &%08lX   &%08lX ---- %s", header->origlen,
#else
			printf("%8d &%08lX   &%08lX ---- %s", header->origlen,
#endif							/* __MSDOS__ */
				   header->load, header->exec,
				   get_comp_desc(header->comptype));
		}
	}
	else
	{
		/* PC archive header */
		date = makedate(header);
		printf("%8ld %02d-%s-%02d %02d:%02d:%02d  ---- %s",
			   header->origlen, date->day, monthname(date->month),
			   date->year + 1900, date->hour, date->minute, date->second,
			   get_comp_desc(header->comptype));
	}
}

/*
 * Test if the given filename matches any of the names specified in the 
 * command line "files list".  This function is also used to test if a given
 * pathname is contained in the any of the names given in the "files list".
 *
 * Returns non-zero if filename matches, or no "files list" exists.
 */
int
inlist(char *filename)
{
	register len = strlen(filename);
	register char **filelist = files;

	if (!*filelist)
		return (1);				/* no "files list" */

	while (*filelist)
#ifdef __MSDOS__
		if (strnicmp(filename, *filelist++, len) == 0)
#else
		if (strncmp(filename, *filelist++, len) == 0)
#endif							/* __MSDOS__ */
			return (1);
	return (0);
}

/*
 * append the file's type to the end if it's name
 * (this function assumes that enough storage is available in the argument
 *  "filename" to store the additional characters ",XXX")
 */

/* BB: For DOS, a comma is of no use. So changed that into a dot. */
int
append_type(Header *header, char *filename)
{
	char append[sizeof(",xxx")];

	/* BB changed constants in next line to long. */
	if ((header->load & (Word) 0xfff00000l) == (Word) 0xfff00000l)
	{
		/* valid time-stamp */
#ifdef __MSDOS__
		sprintf(append, ".%03X", (header->load >> 8) & 0xfff);
#else
		sprintf(append, ",%03X", (header->load >> 8) & 0xfff);
#endif							/* __MSDOS__ */
		strcat(filename, append);
		return (0);
	}
	return (-1);
}

#ifdef DEBUGGING

/*
 * print archive file header info
 */
void
print_header(Header *header)
{
	if (!header)
		return;

	printf("comptype=0X%X name=%s complen=%lu date=0X%X time=0X%X\n",
		   header->comptype, header->name, header->complen, header->date,
		   header->time);
	printf("crc=0X%X origlen=%lu load=0X%lX exec=0X%lX attr=0x%lX\n",
		   header->crc, header->origlen, header->load, header->exec,
		   header->attr);

}
#endif							/* DEBUGGING */
