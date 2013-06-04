# Copyright: (C) 2011 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.7)

project(gsl C)

if (MINGW OR MSYS)
	# We have a lot of object files, so for mingw cmake splits AR line up.
	# That tickles a bug whereby object files of the same name (but from different directories)
	# clobber each other.
	set(AR_WORKAROUND TRUE)
else ()
	set(AR_WORKAROUND FALSE)
endif ()
set(AR_CT 0)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/..)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_MODULE_PATH})

set(SRCS)

macro(ADD_PART lib)
  if (NOT AR_WORKAROUND)
	include_directories(${SRC_DIR}/${lib})
  endif ()
  set(SRCS0)
  foreach(part ${ARGN})
    set(SRC ${SRC_DIR}/${lib}/${part})
	if (AR_WORKAROUND)
		get_filename_component(ext ${SRC} EXT)
		get_filename_component(name ${SRC} NAME_WE)
		get_filename_component(dir ${SRC} PATH)
		get_filename_component(absdir ${dir} ABSOLUTE)
		set(SRC2 ${CMAKE_BINARY_DIR}/flatten/${name}_${AR_CT}${ext})
		math(EXPR AR_CT "${AR_CT}+1")
		configure_file(${SRC} ${SRC2} COPYONLY)
		set_property(SOURCE ${SRC2} APPEND PROPERTY COMPILE_FLAGS "-I${absdir}")
		set(SRC ${SRC2})
	endif()
    set(SRCS0 ${SRCS0} ${SRC})
	#message(STATUS ${SRC})
  endforeach()
  if ("${lib}" STREQUAL "cblas")
	add_library(gslcblas STATIC ${SRCS0})
  else ()
	set(SRCS ${SRCS} ${SRCS0})
  endif ()
endmacro()

include(Headers)
foreach(HDR ${GSL_HEADERS})
  get_filename_component(HDR2 ${HDR} NAME)
  configure_file(${SRC_DIR}/${HDR} ${CMAKE_BINARY_DIR}/include/gsl/${HDR2} COPYONLY)
endforeach ()
configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/include/config.h @ONLY)
include_directories(${CMAKE_BINARY_DIR}/include)
include_directories(${SRC_DIR})

include(Parts)
add_library(gsl STATIC ${SRCS} ${SRC_DIR}/version.c)
add_executable(gsl_test ${SRC_DIR}/block/test.c)
target_link_libraries(gsl_test gsl)
