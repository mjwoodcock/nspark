cmake_minimum_required (VERSION 2.4)
project (nspark C)

add_definitions (-DVERSION=\\"1.7.8B2\\")
add_definitions (-DMAINTAINER=\\"james_woodcock@yahoo.co.uk\\")

SET(NSPARK_LIB_SOURCE arcfs.c compress.c crc.c date.c error.c garble.c io.c misc.c pack.c store.c)

if (UNIX)
	add_definitions (-DPOSIX)
	SET(NSPARK_OS_SOURCE unix.c)
endif (UNIX)

if (WIN32)
	add_definitions (-DWINNT)
	SET(NSPARK_OS_SOURCE winnt.c)
endif (WIN32)

add_executable (nspark arc.c main.c unarc.c ${NSPARK_LIB_SOURCE} ${NSPARK_OS_SOURCE})

if (UNIX)
	add_executable (testprog/testpack testprog/pack.c ${NSPARK_LIB_SOURCE} ${NSPARK_OS_SOURCE})
endif (UNIX)

