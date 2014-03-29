CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(fdbc)

SET(PROJECT_VERSION 0.0.5)

if (CMAKE_COMPILER_IS_GNUCC)

    set(RSN_BASE_C_FLAGS "-Wall -O -g -pedantic -Wshadow -Wunused -Wstrict-aliasing")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${RSN_BASE_C_FLAGS} -DPROJECT_VERSION=\"${PROJECT_VERSION}\"")
  #  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${RSN_BASE_C_FLAGS} -ggdb")
  #  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${RSN_BASE_C_FLAGS}")

    if(APPLE)
        # Newer versions of OSX will spew a bunch of warnings about deprecated ssl functions,
        # this should be addressed at some point in time, but for now, just ignore them.
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_BSD_SOURCE -Wno-deprecated-declarations")
    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
        # XXX Should I set POSIX_C_SOURCE?
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_BSD_SOURCE")
    elseif(UNIX)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_BSD_SOURCE -D_POSIX_C_SOURCE=200112")
    endif(APPLE)

endif(CMAKE_COMPILER_IS_GNUCC)

foreach(loop_var deposit hashtable cluster parser)
    ADD_SUBDIRECTORY(${loop_var})   
endforeach(loop_var)

AUX_SOURCE_DIRECTORY(. DIR_SRCS)
MESSAGE(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})
add_executable(fdbc ${DIR_SRCS})

target_link_libraries(fdbc parser deposit hashtable cluster pthread event z )
