cmake_minimum_required (VERSION 2.4)
if (CMAKE_VERSION GREATER "2.4")
	cmake_policy(SET CMP0005 OLD)
endif (CMAKE_VERSION GREATER "2.4")

if (CMAKE_VERSION GREATER "3.1")
	cmake_policy(SET CMP0054 OLD)
endif (CMAKE_VERSION GREATER "3.1")

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
	add_definitions (-D_CRT_SECURE_NO_WARNINGS)
	SET(NSPARK_OS_SOURCE winnt.c)
endif (WIN32)

if (CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "-Wall")
endif (CMAKE_COMPILER_IS_GNUCC)

add_executable (nspark arc.c main.c unarc.c ${NSPARK_LIB_SOURCE} ${NSPARK_OS_SOURCE})
add_executable (sqsh sqshmain.c unarc.c ${NSPARK_LIB_SOURCE} ${NSPARK_OS_SOURCE})

if (UNIX)
	add_executable (testprog/testpack testprog/pack.c ${NSPARK_LIB_SOURCE} ${NSPARK_OS_SOURCE})
endif (UNIX)

