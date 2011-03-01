cmake_minimum_required(VERSION 2.6)

project(gsl C)

#set(GSL_VERSION $ENV{GSL_VERSION})
set(SRC_DIR ${CMAKE_SOURCE_DIR}/..)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_MODULE_PATH})

set(SRCS)

macro(ADD_PART lib)
  include_directories(${SRC_DIR}/${lib})
  foreach(part ${ARGN})
    set(SRC ${SRC_DIR}/${lib}/${part})
    set(SRCS ${SRCS} ${SRC})
	#message(STATUS ${SRC})
  endforeach()
endmacro()

include(Parts)

include(Headers)
foreach(HDR ${GSL_HEADERS})
  get_filename_component(HDR2 ${HDR} NAME)
  configure_file(${SRC_DIR}/${HDR} ${CMAKE_BINARY_DIR}/include/gsl/${HDR2} COPYONLY)
endforeach ()
configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/include/config.h @ONLY)
include_directories(${CMAKE_BINARY_DIR}/include)
include_directories(${SRC_DIR})

add_library(gsl ${SRCS} ${SRC_DIR}/version.c)
