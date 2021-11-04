cmake_minimum_required (VERSION 2.8)
project (nspark C)

SET(NSPARK_LIB_SOURCE arcfs.c compress.c crc.c date.c error.c garble.c io.c misc.c pack.c store.c)

if (WIN32)
	add_definitions (-DWINNT)
	add_definitions (-D_CRT_SECURE_NO_WARNINGS)
	SET(NSPARK_OS_SOURCE winnt.c)
elseif (RISCOS)
	add_definitions (-DRISCOS)
	SET(NSPARK_OS_SOURCE acorn.c)
	add_compile_options(-mlibscl)
	SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mlibscl")
elseif (UNIX)
	add_definitions (-DPOSIX)
	SET(NSPARK_OS_SOURCE unix.c)
endif ()

if (CMAKE_COMPILER_IS_GNUCC)
	add_compile_options("-Wall")
endif (CMAKE_COMPILER_IS_GNUCC)

add_library (nsparklib ${NSPARK_LIB_SOURCE} ${NSPARK_OS_SOURCE})

add_executable (nspark arc.c main.c unarc.c)
target_link_libraries (nspark LINK_PUBLIC nsparklib)
install (TARGETS nspark DESTINATION bin)

add_executable (sqsh sqshmain.c unarc.c)
target_link_libraries (sqsh LINK_PUBLIC nsparklib)
install (TARGETS sqsh DESTINATION bin)

if (UNIX)
	add_subdirectory (testprog)
endif (UNIX)

install(FILES ${CMAKE_SOURCE_DIR}/man/nspark.1 DESTINATION share/man/man1)
install(FILES ${CMAKE_SOURCE_DIR}/man/sqsh.1 DESTINATION share/man/man1)
install(FILES ${CMAKE_SOURCE_DIR}/man/arcfs.5 DESTINATION share/man/man5)
install(FILES ${CMAKE_SOURCE_DIR}/man/spark.5 DESTINATION share/man/man5)
