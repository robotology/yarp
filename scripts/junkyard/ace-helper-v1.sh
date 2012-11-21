#!/bin/bash

# run as ./scripts/ace-helper.sh

VER=5.6.1
LOCALVER=3

mkdir -p ace-helper
cd ace-helper

ZIP="ACE-$VER.tar.bz2"

if [ ! -e $ZIP ]; then
    echo "Fetching http://download.dre.vanderbilt.edu/previous_versions/$ZIP"
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

YACE=ace4yarp-$VER-$LOCALVER

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
    rm -rf ace/QoS ace/*Reactor ace/SSL
fi

if [ ! -e CMakeLists.txt ]; then

    echo `cat ../ACE_wrappers/ace/Makefile.am | sed "s|^  |  ace/|i"` | sed "s/.*libACE_la_SOURCES = //" | sed "s/libACE_la_LDFLAGS.*//" | sed "s/\\\\/ /g" > inventory.txt
    src=`cat inventory.txt`
    rm inventory.txt

    (
	cat <<EOF 
PROJECT(ACE4YARP)
# Search for source code.
FILE(GLOB_RECURSE folder_source *.cpp *.cc *.c)
FILE(GLOB_RECURSE folder_header *.h)
SOURCE_GROUP("Source Files" FILES \${folder_source})
SOURCE_GROUP("Header Files" FILES \${folder_header})

SET(BUILD_SHARED_LIBS ON)
SET(CONFED FALSE)

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
        # should check for right information, but don't have macs to test on
        # see http://www.cmake.org/Wiki/CMake_Useful_Variables for 
        # possible variables to test
        CONFIGURE_FILE("\${CMAKE_SOURCE_DIR}/ace/config-macosx-tiger.h"
                   "\${CMAKE_BINARY_DIR}/ace/config.h" COPYONLY IMMEDIATE)
        SET(CONFED TRUE)
    ENDIF (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
ENDIF (UNIX)

IF (MINGW)
	MESSAGE(STATUS "MINGW operation")
        CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/ace/config-win32.h"
                   "${CMAKE_BINARY_DIR}/ace/config.h" COPYONLY IMMEDIATE)
        SET(CONFED TRUE)
	ADD_DEFINITIONS(-DACE_HAS_EXCEPTIONS -D__ACE_INLINE__ -DACE_HAS_ACE_TOKEN -DACE_HAS_ACE_SVCCONF -DACE_BUILD_DLL)
	LINK_LIBRARIES(ws2_32 mswsock netapi32)
ENDIF (MINGW)

IF (NOT CONFED)
    MESSAGE(FATALERROR "copy ace/config-YOUR-SYSTEM.h to ace/config.h")
ENDIF (NOT CONFED)


INCLUDE_DIRECTORIES(\${PROJECT_SOURCE_DIR})
EOF
	
	echo "ADD_LIBRARY(ACE4YARP $src \${folder_header})"
	
    ) > CMakeLists.txt


(
cat<<EOF

# Usually, all you'll need to do is set the name of your library here.
# The rest of the file can remain unchanged in most cases.
SET(LIB_TARGET "ACE4YARP")   # name used in ADD_LIBRARY(...)
SET(LIB_PKG "ACE4YARP")      # name you want in FIND_PACKAGE(...)

# Write a message, just so user knows what FIND_PACKAGE is calling
MESSAGE(STATUS "Using \${LIB_PKG}Config.cmake")

# We expect a <LIBRARY>_DIR variable to be available, pointing to
# the directory with this library in it.
SET(LIB_DIR \${\${LIB_PKG}_DIR})

# Tell the user what include directories need to be added to use this library.
SET(\${LIB_PKG}_INCLUDE_DIRS \${LIB_DIR}/include)


# Are we compiling as part of the ICub project, or separately?
IF (NESTED_BUILD)

  # This a global build, so we do not need to supply the full path
  # and filename(s) of the library.  We can just use the CMake target name.
  # CMake itself knows what exactly the library is called on this system.
  SET(\${LIB_PKG}_LIBRARIES \${LIB_TARGET})

ELSE (NESTED_BUILD)

  # this a distributed build, so we have to pin down the library path
  # and filename exactly.

  FIND_LIBRARY(\${LIB_PKG}_LIBRARIES \${LIB_TARGET} \${LIB_DIR})

  IF (NOT \${LIB_PKG}_LIBRARIES)

    # We may be on a system with "Release" and "Debug" sub-configurations
    FIND_LIBRARY(\${LIB_PKG}_LIBRARIES_RELEASE \${LIB_TARGET} 
		 \${LIB_DIR}/Release NO_DEFAULT_PATH)
    FIND_LIBRARY(\${LIB_PKG}_LIBRARIES_DEBUG \${LIB_TARGET} 
		 \${LIB_DIR}/Debug NO_DEFAULT_PATH)

    IF (\${LIB_PKG}_LIBRARIES_RELEASE AND NOT \${LIB_PKG}_LIBRARIES_DEBUG)
	SET(\${LIB_PKG}_LIBRARIES \${\${LIB_PKG}_LIBRARIES_RELEASE} CACHE PATH "release version of library" FORCE)
    ENDIF (\${LIB_PKG}_LIBRARIES_RELEASE AND NOT \${LIB_PKG}_LIBRARIES_DEBUG)

    IF (\${LIB_PKG}_LIBRARIES_DEBUG AND NOT \${LIB_PKG}_LIBRARIES_RELEASE)
	SET(\${LIB_PKG}_LIBRARIES \${\${LIB_PKG}_LIBRARIES_DEBUG} CACHE PATH "debug version of library" FORCE)
    ENDIF (\${LIB_PKG}_LIBRARIES_DEBUG AND NOT \${LIB_PKG}_LIBRARIES_RELEASE)

    IF (\${LIB_PKG}_LIBRARIES_DEBUG AND \${LIB_PKG}_LIBRARIES_RELEASE)
	SET(\${LIB_PKG}_LIBRARIES 
			optimized \${\${LIB_PKG}_LIBRARIES_RELEASE}
			debug \${\${LIB_PKG}_LIBRARIES_DEBUG}  CACHE PATH "debug and release version of library" FORCE)
    ENDIF (\${LIB_PKG}_LIBRARIES_DEBUG AND \${LIB_PKG}_LIBRARIES_RELEASE)

  ENDIF (NOT \${LIB_PKG}_LIBRARIES)

ENDIF (NESTED_BUILD)
EOF
) > ACE4YARPConfig.cmake
fi

cd ..


#rm -f $YACE.tar $YACE.tar.gz $YACE.zip

if [ ! -e $YACE.tar.gz ] ; then
    echo Generating $YACE.tar.gz
    rm -f $YACE.tar $YACE.tar.gz
    tar -cvvf $YACE.tar $YACE
    gzip $YACE.tar
fi


if [ ! -e $YACE.zip ]; then
    echo Generating $YACE.zip
    rm -rf win
    mkdir -p win
    cd win
    cp -R ../$YACE $YACE
    # tofrodos - Converts DOS <-> Unix text files, alias tofromdos
    find $YACE -type f -exec unix2dos {} \;
    zip -r ../$YACE.zip $YACE
    cd ..
fi

if [ ! -e sent-$YACE.txt ]; then
    scp $YACE.zip $YACE.tar.gz wiki.icub.org:/var/www/html/yarp/downloads/yarp && ( echo done > sent-$YACE.txt )
fi
