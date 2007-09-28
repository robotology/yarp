#!/bin/bash

# run as ./scripts/ace-helper.sh

VER=5.6.1

mkdir -p ace-helper
cd ace-helper

ZIP="ACE-$VER.tar.bz2"

if [ ! -e $ZIP ]; then
    wget http://download.dre.vanderbilt.edu/previous_versions/$ZIP
    rm -rf ACE_wrappers
fi

if [ ! -e $ZIP ]; then
    echo Could not fetch $ZIP
    exit 1
fi

if [ ! -e ACE_wrappers ]; then
    tar xjvf $ZIP
fi

if [ ! -e ACE_wrappers ]; then
    echo Could not unpack $ZIP
    exit 1
fi

YACE=ACE_yappers

if [ ! -e $YACE ]; then
    mkdir -p $YACE
fi

if [ ! -e $YACE ]; then
    echo "Cannot create $YACE"
    exit 1
fi

cd $YACE

if [ ! -e COPYING ]; then
    cp ../ACE_wrappers/COPYING .
fi

if [ ! -e ace ]; then
    cp -R ../ACE_wrappers/ace ace
    rm -rf ace/QoS
fi

if [ ! -e CMakeLists.txt ]; then

    echo `cat ../ACE_wrappers/ace/Makefile.am | sed "s|^  |  ace/|i"` | sed "s/.*libACE_la_SOURCES = //" | sed "s/libACE_la_LDFLAGS.*//" | sed "s/\\\\/ /g" > inventory.txt
    src=`cat inventory.txt`

    (
	cat <<EOF 
PROJECT(ACE4YARP)
# Search for source code.
FILE(GLOB_RECURSE folder_source *.cpp *.cc *.c)
FILE(GLOB_RECURSE folder_header *.h)
SOURCE_GROUP("Source Files" FILES \${folder_source})
SOURCE_GROUP("Header Files" FILES \${folder_header})

SET(CONFED FALSE)

IF (NOT EXISTS "\${CMAKE_SOURCE_DIR}/ace/config.h")

IF (MSVC)
    CONFIGURE_FILE("\${CMAKE_SOURCE_DIR}/ace/config-win32.h"
                   "\${CMAKE_BINARY_DIR}/ace/config.h" COPYONLY IMMEDIATE)
    SET(CONFED TRUE)
ENDIF (MSVC)

IF (UNIX) 
    IF (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        CONFIGURE_FILE("\${CMAKE_SOURCE_DIR}/ace/config-linux.h"
                   "\${CMAKE_BINARY_DIR}/ace/config.h" COPYONLY IMMEDIATE)
        SET(CONFED TRUE)
    ENDIF (CMAKE_SYSTEM_NAME STREQUAL "Linux")

    IF (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        CONFIGURE_FILE("\${CMAKE_SOURCE_DIR}/ace/config-linux.h"
                   "\${CMAKE_BINARY_DIR}/ace/config.h" COPYONLY IMMEDIATE)
        SET(CONFED TRUE)
    ENDIF (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        # should check for right information, but don't have macs to test on
        # see http://www.cmake.org/Wiki/CMake_Useful_Variables for 
        # possible variables to test
        CONFIGURE_FILE("\${CMAKE_SOURCE_DIR}/ace/config-macosx-tiger.h"
                   "\${CMAKE_BINARY_DIR}/ace/config.h" COPYONLY IMMEDIATE)
        SET(CONFED TRUE)
    ENDIF (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
ENDIF (UNIX)

IF (NOT CONFED)
    MESSAGE(FATALERROR "copy ace/config-YOUR-SYSTEM.h to ace/config.h")
ENDIF (NOT CONFED)


ENDIF (NOT EXISTS "\${CMAKE_SOURCE_DIR}/ace/config.h")

INCLUDE_DIRECTORIES(\${PROJECT_SOURCE_DIR})
EOF
	
	echo "ADD_LIBRARY(ACE4YARP $src \${folder_header})"
	
    ) > CMakeLists.txt


(
cat<<EOF
IF (NOT ACE4YARP_FOUND)
  FIND_LIBRARY(ACE4YARP_LIBRARIES ACE4YARP PATHS /usr/lib ${ACE4YARP_DIR} ${ACE_DIR} $ENV{ACE_ROOT})
  FIND_PATH(ACE4YARP_INCLUDE_DIRS ace/config.h /usr/include/ACE4YARP  ${ACE4YARP_DIR} ${ACE_DIR} $ENV{ACE_ROOT})
  IF (ACE4YARP_LIBRARIES)
    SET (ACE4YARP_FOUND TRUE)
  ENDIF (ACE4YARP_LIBRARIES)
ENDIF (NOT ACE4YARP_FOUND)
EOF
) > ACE4YARPConfig.cmake
fi



