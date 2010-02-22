#
# NOTE:  NSPARK DOESN'T WORK UNDER MS-DOS RIGHT NOW... COMPRESS.C NEEDS
#        CALLS TO VMALLOC() ET AL TO ALLOW DATA AREAS > 64KB TO BE ALLOCATED.
#
# nspark makefile (MSDOS, Microsoft V7.0 C Compiler)
#
# SYSTYPE:	MSDOS2, MSDOS3, MSDOS4, MSDOS5, MSDOS6, MSDOS7.
#
SYSTYPE= MSDOS3 # using DRDOS 6 == API level 3.31

INSTALLDIR= d:\util

# BB added next two lines
NAME = nspark
CONFIGFILE = $(NAME).cfg

# Borland C/C++
CC=BCC +$(CONFIGFILE)
LD=TLINK

# Change the next two lines to your own local settings.
#
# If you want to use the settings in your turboc.cfg file,
# remove LIBPATH and INCLUDEPATH from the definition of
# $(CONFIGFILE) and change the above definition for CC
# to CC=BCC @$(CONFIGFILE). (Not recommended since there may
# be other unwanted options set in turboc.cfg.)
LIBPATH = H:\BORLANDC\LIB
INCLUDEPATH = H:\BORLANDC\INCLUDE

# Uncomment ONE of the next two lines when you have PKLITE or LZEXE.
# pklite: optionally add -e to options if you have the professional
# version.
COMPRESS = d:\util\pklite -b
# COMPRESS = d:\util\lzexe

# --- You shouldn't need to edit below this line ---
# BB: But only if you are using Borland C/C++!

# NB: Spaces are not allowed in -D arguments.
VERSION= 1.7.6_dos-beta
MAINTAINER= bob@wop.wtb.tue.nl_(this_beta_only!)

# BB commented out next three lines for Borland C/C++: use response file
#CDEFINES= -mc -a -f- -w+ -O2 -k- -d -v- -vi- -H=$(NAME).SYM -UMSDOS
#CMISC=	-DVERSION="$(VERSION)" -DMAINTAINER="$(MAINTAINER)" -D$(SYSTYPE)
#CFLAGS=	$(CDEFINES) $(CMISC)
# Borland C/C++ uses TLINK.
LDFLAGS= /x/c/P-
# BB changed next line
#PROG=	nspark.exe
PROG=	$(NAME).exe
SRCS=	main.c arc.c unarc.c store.c pack.c compress.c crc.c \
	io.c error.c misc.c date.c msdos.c arcfs.c
HDRS=	main.h arc.h unarc.h store.h pack.h compress.h crc.h \
	io.h error.h misc.h date.h os.h spark.h arcfs.h
OBJS=	main.obj arc.obj unarc.obj store.obj pack.obj compress.obj crc.obj \
	io.obj error.obj misc.obj date.obj msdos.obj arcfs.obj
LIBS=	

# BB overrule default implicite rule.
.c.obj:
	$(CC) -c {$< }

# BB changed next line
#all:	$(PROG)
.AUTODEPEND:

all:	$(CONFIGFILE) $(PROG)

$(PROG): $(CONFIGFILE) $(OBJS)
	# BB commented out next line
	# set LINK=$(LDFLAGS)
	# BB changed next line
	# $(LD) $(OBJS), $(PROG);
	$(LD) $(LDFLAGS) @&&|
c0c.obj+ # Using compact memory model
arc.obj+
arcfs.obj+
compress.obj+
crc.obj+
date.obj+
error.obj+
io.obj+
main.obj+
misc.obj+
msdos.obj+
pack.obj+
store.obj+
unarc.obj
$(NAME)
		# no map file
cc.lib # using compact memory model
|
!ifdef COMPRESS
	# create directory uncompr for uncompressed .exe
	if not exist uncompr\nul md uncompr
	copy $(PROG) uncompr\$(PROG)
	$(COMPRESS) $(PROG)
!endif


$(CONFIGFILE): makefile
	copy &&|
-mc # using compact memory model
-a
-f-
-w+
-O2
-k-
-d
-v-
-vi-
-H=$(NAME).SYM
# BB remove next two lines when you are using a turboc.cfg file.
# and change CC to BCC @$(CONFIGFILE).
-I$(INCLUDEPATH)
-L$(LIBPATH)
-DMSDOS3
-DVERSION="$(VERSION)"
-DMAINTAINER="$(MAINTAINER)"
-UMSDOS
| $(CONFIGFILE)

install: $(PROG)
	copy $(PROG) $(INSTALLDIR)

clean:
	del $(PROG)
	del *.obj 
	del *.bak
	del *.map
	del mklog
	# BB added next four lines
	del *.bk?
	del $(CONFIGFILE)
	del $(NAME).sym
	del *.$$$
