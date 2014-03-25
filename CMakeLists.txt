CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(fdbc)

ADD_SUBDIRECTORY(deposit)
ADD_SUBDIRECTORY(hashtable)
ADD_SUBDIRECTORY(parser)

AUX_SOURCE_DIRECTORY(. DIR_SRCS)
MESSAGE(STATUS "This is BINARY dir " ${HELLO_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir "${HELLO_SOURCE_DIR})
add_executable(fdbc ${DIR_SRCS})

target_link_libraries(fdbc parser deposit hashtable pthread event z )
