#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spark.h"
#include "arcfs.h"
#include "unarc.h"
#include "error.h"

char *ourname;
unsigned char apptype = 0;
unsigned char force = 0;
unsigned char inffiles = 0;
unsigned char listing = 0;
unsigned char testing = 0;
unsigned char quiet = 1;
unsigned char retry = 0;
unsigned char singlecase = 0;
unsigned char stamp = 1;
unsigned char to_stdout = 0;
unsigned char verbose = 0;
char *archive = NULL;
char **files = NULL;
char *logfile = NULL;

void
usage()
{
	fprintf(stderr, "sqsh: un-squashes RISC OS squashed files\n");
	fprintf(stderr, "Usage: sqsh [options] <filename>\n");
	fprintf(stderr, "	where options are:\n");
	fprintf(stderr, "       -c output to stdout\n");
	fprintf(stderr, "       -f force file overwrite\n");
	fprintf(stderr, "       -s do not set timestamp of decompressed file\n");
	exit(1);
}

int
main(int argc, char *argv[])
{
	int r;
	int i;

	if (argc < 2)
	{
		usage();
	}

	ourname = argv[0];
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
				case 'c':
					to_stdout = 1;
					break;
				case 'f':
					force = 1;
					break;
				case 's':
					stamp = 0;
					break;
			}
		}
		else
		{
			break;
		}
	}

	if (i == argc)
	{
		usage();
	}
	archive = argv[i];

	r = do_unsquash();
	if (r != 0 || writesize > 0)
	{
		error("Error unsquashing file");
	}

	return r;
}
