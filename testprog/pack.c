#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../spark.h"
#include "../crc.h"
#include "../nsparkio.h"
#include "../pack.h"
#include "../arcfs.h"
#include "../garble.h"
#include "../error.h"

char **files = NULL;
char *archive = NULL;
char *ourname = NULL;

unsigned char testing = 0;
unsigned char quiet = 0;
unsigned int singlecase = 0;

int
main(int argc, char *argv[])
{
	FILE *in, *out;
	Header header;
	int r;

	memset(&header, 0, sizeof(header));

	set_password("james");

	if (argc != 4)
	{
		fprintf(stderr, "usage: %s <-p|u> <infile> <outfile>\n", argv[0]);
		exit(1);
	}

	in = fopen(argv[2], "r");
	if (!in)
	{
		error("Failed to open %s for reading", argv[2]);
		exit(1);
	}

	out = fopen(argv[3], "w");
	if (!out)
	{
		error("Failed to open %s for writing", argv[3]);
		exit(1);
	}

	if (strcmp(argv[1], "-p") == 0)
	{
		fseek(in, 0, SEEK_END);
		header.origlen = ftell(in);
		fseek(in, 0, SEEK_SET);
		fwrite(&header, sizeof(header), 1, out);
		crcsize = header.origlen;
		pack(&header, in, out);
		fseek(out, 0, SEEK_SET);
		msg("\n");
		fwrite(&header, sizeof(header), 1, out);
		msg("Original size = %d\n", header.origlen);
		msg("Compressed size = %d\n", header.complen);
	}
	else
	{
		if (fread(&header, sizeof(header), 1, in)!=sizeof(header)) {
			perror("Read failed!");
			exit(1);
		}
		msg("Original size = %d\n", header.origlen);
		msg("Compressed size = %d\n", header.complen);
		crcsize = writesize = header.origlen;
		r = unpack(&header, in, out);
		if (r != NOERR)
		{
			error("Failed %d", r);
		}
		msg("\n");
	}

	fclose(in);
	fclose(out);
	return 0;
}
