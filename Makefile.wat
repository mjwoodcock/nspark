SYSTYPE = WINNT
CC = wcc386

SRCS=	store.c pack.c compress.c crc.c &
	io.c error.c misc.c date.c arcfs.c garble.c
HDRS=	store.h pack.h compress.h crc.h &
	nsparkio.h error.h misc.h date.h os.h spark.h arcfs.h garble.h version.h

NSPARKSRCS = main.c arc.c unarc.c
NSPARKHDRS = main.h arc.h unarc.h

SQSHSRCS = sqshmain.c unarc.c
SQSHHDRS = unarc.h

EXE = .exe
OBJ = .obj

!ifeq SYSTYPE WINNT
CFLAGS += -bt=nt
OS_SRCS = winnt.c
system = nt
!endif

!ifeq SYSTYPE MSDOS
CFLAGS += -D__MSDOS__ -bt=dos -mc
OS_SRCS = msdos.c
system = dos
CC=wcc
!endif

!ifeq SYSTYPE MSDOS32
CFLAGS += -D__MSDOS__ -bt=dos
OS_SRCS = msdos.c
system = dos4g
!endif

OBJS = $(SRCS:.c=$(OBJ))
NSPARKOBJS = $(NSPARKSRCS:.c=$(OBJ))
SQSHOBJS = $(SQSHSRCS:.c=$(OBJ))
OS_OBJS = $(OS_SRCS:.c=$(OBJ))

CFLAGS += -zq -oaxt -d0 -wx -fo=$(OBJ) -D$(SYSTYPE)

all: nspark$(EXE) sqsh$(EXE)

nspark$(EXE) : nspark.lnk
	wlink @$?

sqsh$(EXE) : sqsh.lnk
	wlink @$?

nspark.lnk : $(OBJS) $(OS_OBJS) $(NSPARKOBJS)
    %create $@
    @%append $@ debug all
    @%append $@ name $*
    @%append $@ op map, quiet
    @%append $@ system $(system)
    @for %i in ($?) do @%append $@ file %i

sqsh.lnk : $(OBJS) $(OS_OBJS) $(SQSHOBJS)
    %create sqsh.lnk
    @%append $@ debug all
    @%append $@ name $*
    @%append $@ op map, quiet
    @%append $@ system $(system)
    @for %i in ($?) do @%append $@ file %i

.EXTENSIONS:
.EXTENSIONS: $(EXE) $(OBJ) .c .h

.c:$(CDIR)

.c$(OBJ) :
        $(CC) $(CFLAGS) $[*

clean: .symbolic
    rm -f *$(EXE) *$(OBJ) *.err *.lnk *.exp *.map
