
/*
 * main function
 *
 * Revision 1.12 99/03/17 MU
 * added the option -i to extract with .inf files
 * Also added the .inf extension to the default logfile name for MSDOS
 *
 * $Header: main.c 1.11 95/08/01 $
 * $Log:	main.c,v $
 * Revision 1.11  95/08/01  xx:xx:xx  BB
 * Fixed for Borland C/C++
 * Added / as command line switch option for DOS
 *
 * Revision 1.10  93/08/20  12:39:28  arb
 * Added support for ArcFS archive detection
 *
 * Revision 1.9  93/08/20  10:30:50  arb
 * Added -C option for "convert filenames to lowercase"
 *
 * Revision 1.8  93/03/05  15:40:32  arb
 * Added <stdlib.h> for RISCOS, needed for exit()
 *
 * Revision 1.7  92/12/09  09:43:03  duplain
 * Changed "-a" option to "-T".
 *
 * Revision 1.6  92/12/08  10:19:30  duplain
 * Added -a option for "append filetype".
 *
 * Revision 1.5  92/12/07  17:18:42  duplain
 * reformatted source.
 *
 * Revision 1.4  92/10/19  09:33:09  duplain
 * Added -x as an alternative to -u.
 *
 * Revision 1.3  92/10/01  11:21:39  duplain
 * Added -R option.
 *
 * Revision 1.2  92/09/30  10:27:29  duplain
 * Added logfile option and processing.
 *
 * Revision 1.1  92/09/29  18:02:20  duplain
 * Initial revision
 *
 */  
	
#include <stdio.h>
#include <ctype.h>
#include "spark.h"

/* BB changed next line */ 

/* #include "io.h" */ 
#include "nsparkio.h"
#include "error.h"
#include "misc.h"
#include "arc.h"
#include "unarc.h"
#include "garble.h"

/* BB changed next line */ 

/* #ifdef RISCOS */ 
#if defined(RISCOS) || defined(__MSDOS__)
#include <stdlib.h>
#endif	/* RISCOS || __MSDOS__ */

/* BB added next includes */ 
#ifdef __MSDOS__
#include <ctype.h>
#include <string.h>
#endif	/* __MSDOS__ */
	
char *ourname;					/* program name */
char *archive;					/* name of archive file */


/* MU changed the default log filename for MSDOS */ 
#ifdef __MSDOS__
char *logfile = "settypes.inf";	/* default name for log file */

#else	/*  */
char *logfile = "settypes";		/* default name for log file */

#endif	/*  */
char **files;					/* optional file arguments */
unsigned char unarc = 0;		/* -u or -x or I */
unsigned char inffiles = 0;	/* I */
unsigned char quiet = 0;		/* -q */
unsigned char verbose = 0;		/* -v */
unsigned char testing = 0;		/* -t */
unsigned char listing = 0;		/* -l */
unsigned char force = 0;		/* -f */
unsigned char stamp = 1;		/* -s */
unsigned char retry = 0;		/* -R */
unsigned char apptype = 0;		/* -T */
unsigned char singlecase = 0;	/* -C */

#ifdef DEBUGGING
unsigned char debugging = 0;	/* -D */

#endif	/* DEBUGGING */
void usage P__((void));
int do_unarc P__((void));
int do_arc P__((void));

int
main(argc, argv)
	int argc;
	char **argv;
{
	register i;
	
		/* BB extra switch indicator for command line parsing
		   DOS allows switches like /a/b/c which enters as one
		   `word' in argv. */ 
#ifdef __MSDOS__
	int nextmaybeaswitch;
	
#endif	/*  */
		
#ifdef DEBUGGING
		/*
		 * check types are defined correctly for this machine (or compiler ???)
		 */ 
		if (sizeof(Word) != 4)
	{
		puts("Word size != 4");
		exit(1);
	}
	if (sizeof(Halfword) != 2)
	{
		puts("Halfword size != 2");
		exit(1);
	}
	if (sizeof(Byte) != 1)
	{
		puts("Byte size != 1");
		exit(1);
	}
	
#endif	/* DEBUGGING */
		ourname = basename(*argv++);
	
		/* BB cosmetics for DOS: Strip extention .exe (or .com if
		   somebody would like to compile nspark to a .com) from
		   the ourname string. And convert it to lowercase. That
		   way it looks better than ``NSPARK.EXE: error ...''. */ 
#ifdef __MSDOS__
		/* For DOS prior to 3.0, argv[0] contains the NULL pointer.
		   So substitute a `default'. */ 
		if (!ourname)
		ourname = "nspark";
	
		/* NB: stricmp == strcmpi == strcasecmp */ 
		if (stricmp(&ourname[strlen(ourname) - 4], ".com") == 0
			|| stricmp(&ourname[strlen(ourname) - 4], ".exe") == 0)
	{
		
			/* We cannot write a '\0' into ourname because it points to
			   argv[0]. And that may be in a read-only data segment. */ 
		char *newname, *cp;
		if ((newname = (char *) malloc(strlen(ourname)) - 3) != NULL)
		{
			strncpy(newname, ourname, strlen(ourname) - 4);
			newname[strlen(ourname) - 4] = '\0';
			ourname = newname;	/* Allocated space will be released
								   automatically at exit */
			for (cp = ourname; *cp; cp++)
				if (isascii(*cp) && isupper(*cp))
					*cp = tolower(*cp);
		}
	}
	
#endif	/* __MSDOS__ */
		argc--;
	
		/*
		 * parse args (can't use getopt() 'cos not all C libraries have it)
		 */ 
		while (argc)
	{
		int donext = 0;
		char *arg = *argv;
		
			/* BB changed next line */ 
			/* if (*arg == '-') { */ 
#ifdef __MSDOS__
			if (*arg == '-' || *arg == '/')
		{
			
#else	/*  */
			if (*arg == '-')
		{
			
#endif	/* __MSDOS__ */
			char c;
			
#ifdef __MSDOS__
				/* BB first char following a switch may not be another switch */ 
				nextmaybeaswitch = 0;
			
#endif	/* __MSDOS__ */
				while (!donext && !isspace(c = *++arg) && c)
			{
				switch (c)
				{
				case 'u':
				case 'x':
					unarc = 1;
					break;
				case 't':
					testing++;
					unarc = 1;	/* implied */
					break;
				case 'l':
					listing++;
					unarc = 1;	/* implied */
					break;
				case 'q':
					quiet = 1;
					break;
				case 'v':
					verbose = 1;
					break;
				case 'f':
					force = 1;
					break;
				case 's':
					stamp = 0;
					break;
				case 'R':
					retry = 1;
					break;
				case 'V':
					printf("%s v%s - maintained by %s - PUBLIC DOMAIN\n",
							ourname, VERSION, MAINTAINER);
					break;
				case 'T':
					apptype = 1;
					logfile = NULL;
					break;
				case 'C':
					singlecase = 1;
					break;
				case 'L':
					if (!apptype)
					{
						if (*++arg)
							logfile = arg;
					
						else
						if (--argc)
							logfile = *++argv;
					
						else
							usage();
					}
					donext++;
					break;
					
#ifdef DEBUGGING
				case 'D':
					debugging = 1;
					break;
					
#endif	/* DEBUGGING */
#ifdef __MSDOS__
						/* BB DOS allows switches like /a/b/c */ 
				case '/':
					if (nextmaybeaswitch && arg[1] != '/')
						break;
					
					else		/* fall through to error message */
						;
					
#endif	/* __MSDOS__ */
				case 'I':
					unarc = 1;
					inffiles = 1;
					break;
				case 'p':
					if (*++arg)
						set_password(arg);
					else
						if (--argc)
							set_password(*++argv);
						else
							usage();
					donext++;
					break;
				default:
					error("unknown option '%c'", c);
					exit(1);
				}
				
#ifdef __MSDOS__
					/* BB We've had a valid switch, next char may be
					   a / again */ 
					nextmaybeaswitch = 1;
				
#endif	/* __MSDOS__ */
			}
			argv++;
			argc--;
		}
		else
			break;
	}
	if (!argc)
		usage();
	archive = *argv++;
	files = argv;
	if (unarc)
		i = do_unarc();
	
	else
		i = do_arc();
	exit(i);
	
		/* BB added next line */ 
		return 0;				/* Keep compiler happy. */
}



/*
 * display program usage and exit
 */ 
void
usage() 
{
	fprintf(stderr, "usage: %s [options] archive [file ... file]\n",
			 ourname);
	fprintf(stderr, "       where options are:\n");
	fprintf(stderr,
			 "       -u or -x unarchive           -t test archive integrity\n");
	fprintf(stderr, "       -l list archive contents     -q quiet\n");
	fprintf(stderr,
			 "       -f force file overwrite      -s no filestamp\n");
	fprintf(stderr,
			 "       -v verbose                   -V display version number\n");
	fprintf(stderr,
			 "       -R retry if archive corrupt  -L<name> set logfile name\n");
	fprintf(stderr,
			 "       -T append filetype to name   -C create lowercase filenames\n");
	
		/* MU added instuctions for the -I option */ 
	fprintf(stderr, "       -I unarchive with .inf files -p<password> set password\n");
	exit(1);
} 
