#
# nspark makefile (Acorn)
#
# $Header: makefile 1.30 92/12/23 $
#
# SYSTYPE:	RISCOS2, RISCOS3
#
SYSTYPE= RISCOS2
INSTALLDIR= $${HOME}/bin
CC = cc

# Under System V change "install" to "cp"
INSTALL = install -s

# --- You shouldn't need to edit below this line ---

VERSION = 1.7.6
MAINTAINER = arb@comp.lancs.ac.uk
BACKUPFILE = $${HOME}/tmp/nspark.zip

CDEFINES = -I. -O
CMISC = -DVERSION="\"$(VERSION)\"" -DMAINTAINER="\"$(MAINTAINER)\"" -D$(SYSTYPE)
CFLAGS = $(CDEFINES) $(CMISC) -throwback
PROG = nspark
SRCS = main.c arc.c unarc.c store.c pack.c compress.c crc.c \
	io.c error.c misc.c date.c acorn.c arcfs.c
HDRS = main.h arc.h unarc.h store.h pack.h compress.h crc.h \
	nsparkio.h error.h misc.h date.h os.h spark.h arcfs.h
OBJS = main.o arc.o unarc.o store.o pack.o compress.o crc.o \
	io.o error.o misc.o date.o acorn.o arcfs.o
LIBS =

.DEFAULT:
	co -S $<

all:	$(PROG)

$(PROG): $(OBJS)
	$(CC) -o $(PROG) $(OBJS) $(LIBS)

install: $(PROG)
	 $(INSTALL) $(PROG) $(INSTALLDIR)

depend: $(SRCS) $(HDRS)
	makedepend -w200 $(CDEFINES) -D$(SYSTYPE)  -- $(SRCS)
	mv makefile makefile.old
	sed -f nosysdep.sed < makefile.old > makefile
	rm -f makefile.old makefile.bak

tags:	$(SRCS) $(HDRS)
	ctags $(SRCS) $(HDRS)

clean:
	rm -f $(PROG) $(OBJS) mkendian endian.h
	rm -f a.out core *~

backup:	clean
	(cd ..; zip -r9y $(BACKUPFILE) nspark)

TMPDIR = $$HOME/tmp
DISTLEAF = nspark-$(VERSION)
DISTDIR = $(TMPDIR)/$(DISTLEAF)
dist:	clean $(SRCS) $(HDRS)
	rm -f RCS/rel-$(VERSION)
	makerel makefile *.c *.h > RCS/rel-$(VERSION)
	chmod 444 RCS/rel-$(VERSION)
	mkdir $(DISTDIR)
	cp *.c *.h nosysdep.sed README BUGS CHANGES makefile $(DISTDIR)
	mkdir $(DISTDIR)/mkfiles
	cp mkfiles/* $(DISTDIR)/mkfiles
	(cd $(TMPDIR); tar cf - $(DISTLEAF) | compress > $(DISTLEAF).tar.Z)
	(cd $(TMPDIR); rm -rf $(DISTLEAF))

io.o:	endian.h

endian.h:	mkendian
	mkendian > h.endian

mkendian:	mkendian.c
	$(CC) -o mkendian mkendian.c

.c.o:
    $(CC) $(CFLAGS) -c $*.c

# DO NOT DELETE THIS LINE -- make depend depends on it.

main.o: spark.h nsparkio.h error.h misc.h arc.h unarc.h
arc.o: spark.h error.h
unarc.o: spark.h store.h pack.h compress.h main.h nsparkio.h misc.h os.h error.h crc.h
store.o: spark.h main.h crc.h nsparkio.h
pack.o: spark.h main.h crc.h nsparkio.h
compress.o: spark.h pack.h main.h crc.h nsparkio.h
crc.o: spark.h main.h
io.o: spark.h main.h error.h arcfs.h
error.o: error.h
misc.o: spark.h main.h date.h misc.h
date.o: spark.h
