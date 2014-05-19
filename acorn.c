
/*
 * Operating System specific function (Acorn)
 *
 * $Header: acorn.c 1.1 93/03/05 $
 * $Log:	acorn.c,v $
 * Revision 1.1  93/03/05  14:34:04  arb
 * Initial revision
 * 
 */

#include "spark.h"
#include "date.h"
#include "kernel.h"
#include "swis.h"

/*
 * return the length of a file
 */
Word
filesize(pathname)
	char *pathname;
{
	int rc;
	_kernel_osfile_block osblock;

	rc = _kernel_osfile(17, pathname, &osblock);
	if (rc == _kernel_ERROR)
		return (0);
	/* Bit 1 is set if a file, bit 2 if a directory, both if image file */
	if (rc & 1)
		return ((Word) osblock.start);
	else
		return (0);
}

/*
 * test for the existance of a file or directory
 */
Ftype
exist(pathname)
	char *pathname;
{
	int rc;
	_kernel_osfile_block osblock;

	rc = _kernel_osfile(17, pathname, &osblock);
	if (rc == _kernel_ERROR)
		return (NOEXIST);		/* assumes error was because file
								   doesn't exist... could be wrong! */
	if (rc & 1)
		return (ISFILE);		/* might not be a regular file... (eg. image) */
	if (rc & 2)
		return (ISDIR);
	else
		return (NOEXIST);
}

/*
 * make a directory
 */
int
makedir(pathname)
	char *pathname;
{
	int rc;
	_kernel_osfile_block osblock;

	osblock.start = 0;			/* Default number of entries */
	rc = _kernel_osfile(8, pathname, &osblock);
	if (rc == _kernel_ERROR)
		return (-1);			/* Should set errno */
	else
		return (0);
}

/*
 * stamp a file with date and time
 */
int
filestamp(header, filename)
	Header *header;
	char *filename;
{
	int rc;
	_kernel_osfile_block osblock;

	if ((header->load & (Word) 0xfff00000) != (Word) 0xfff00000)
		return (0);				/* not a timestamp */

	osblock.load = header->load;
	osblock.exec = header->exec;
	osblock.end = header->attr;
	rc = _kernel_osfile(1, filename, &osblock);
	if (rc == _kernel_ERROR)
		return (-1);
	else
		return (0);
}

/*
 * read byte from stream (only one line from stdin supported)
 */
int
read(fd, buffer, len)
	int fd;
	void *buffer;
	int len;
{
	_kernel_swi_regs regs;
	int carry;

	if (fd != 0)				/* Only stdin, sorry! */
		return (0);
	regs.r[0] = (int) buffer;
	regs.r[1] = len;
	regs.r[2] = 1;				/* Low/high values to store */
	regs.r[3] = 255;
	if (_kernel_swi_c(OS_ReadLine, &regs, &regs, &carry))
		return (-1);			/* Error occurred */
	if (carry)
		return (-1);			/* Escape pressed */
	return (regs.r[1]);			/* Return number of bytes read */
}
