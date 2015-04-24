
/*
 * Determine byte-sex of local host to enable RISC OS (little endian) files
 * to be read/written.
 *
 * $Header: mkendian.c 1.3 95/01/06 $
 * $Log:	mkendian.c,v $
 * Revision 1.3  95/01/06  12:02:00  arb
 * Fixes for Alpha.
 *
 * Revision 1.2  92/12/07  17:19:05  duplain
 * reformatted source.
 * 
 * Revision 1.1  92/09/29  18:02:23  duplain
 * Initial revision
 * 
 */

#include <stdio.h>
#include <stdlib.h>

void
main()
{
	register i;
	union
	{
#if defined(__alpha) || defined(__x86_64__)
		unsigned int l;
#else
		unsigned long l;
#endif
		unsigned short w;
		char c[4];
	} x;

	if (sizeof(x.l) != 4 || sizeof(x.w) != 2)
	{
		fprintf(stderr, "size mismatch in union... aborting\n");
		exit(1);
	}

	puts("/* endian.h:  local host specific byte-sex defines */");
	puts("#ifndef __NSENDIAN_H");
	puts("#define __NSENDIAN_H");

	x.l = 0x00010203;
	if (x.c[0] == 3 && x.c[1] == 2 && x.c[2] == 1 && x.c[3] == 0)
		puts("#define NS_LITTLE_ENDIAN");
	else
		puts("#undef NS_LITTLE_ENDIAN");
	for (i = 0; i < 4; i++)
		printf("#define WORD%d\t%d\n", i, 3 - x.c[i]);

	x.w = 0x0001;
	for (i = 0; i < 2; i++)
		printf("#define HALFWORD%d\t%d\n", i, 1 - x.c[i]);

	puts("#endif /* __ENDIAN_H */");
	exit(0);
}
