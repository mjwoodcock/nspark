cmake_minimum_required (VERSION 2.4)
if (CMAKE_VERSION GREATER "2.4")
	cmake_policy(SET CMP0005 OLD)
endif (CMAKE_VERSION GREATER "2.4")

if (CMAKE_VERSION GREATER "3.1")
	cmake_policy(SET CMP0054 OLD)
endif (CMAKE_VERSION GREATER "3.1")

project (nspark C)

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

add_library (nsparklib ${NSPARK_LIB_SOURCE} ${NSPARK_OS_SOURCE})
add_executable (nspark arc.c main.c unarc.c)
target_link_libraries (nspark LINK_PUBLIC nsparklib)
add_executable (sqsh sqshmain.c unarc.c)
target_link_libraries (sqsh LINK_PUBLIC nsparklib)

if (UNIX)
	add_subdirectory (testprog)
endif (UNIX)

