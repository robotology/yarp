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

if [ ! -e ACE_COPYING ]; then
    cp ../ACE_wrappers/COPYING ACE_COPYING
fi

if [ ! -e ace_orig ]; then
    cp -R ../ACE_wrappers/ace ace_orig
    rm -rf ace_orig/QoS ace/*Reactor ace_orig/SSL
fi

if [ ! -e ace_sources.txt ]; then
    echo "Enumerating sources"

    echo `cat ../ACE_wrappers/ace/Makefile.am | sed "s|^  |  ace/|i"` | sed "s/.*libACE_la_SOURCES = //" | sed "s/libACE_la_LDFLAGS.*//" | sed "s/\\\\/ /g" > ace_sources.txt
    src=`cat ace_sources.txt`
fi

if [ ! -e CMakeLists.txt.template ]; then
    echo "Making template CMakeLists.txt.template, in case stand-alone compile is desired"

    src=`cat ace_sources.txt`

    (
	cat <<EOF 
PROJECT(ACE4YARP)
# Search for source code.
FILE(GLOB_RECURSE folder_source src/libYARP_OS/src/*.cpp src/libYARP_OS/src/*.cc src/libYARP_OS/src/*.c)
FILE(GLOB_RECURSE folder_header include/*.h)
SOURCE_GROUP("Source Files" FILES \${folder_source})
SOURCE_GROUP("Header Files" FILES \${folder_header})

INCLUDE_DIRECTORIES(\${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(BEFORE \${PROJECT_SOURCE_DIR}/src/libYARP_OS/include)

IF (MINGW)
  ADD_DEFINITIONS(-D__MINGW32__ -D__MINGW__)
ENDIF (MINGW)

ADD_LIBRARY(ACE4YARP \${folder_source} \${folder_header})

EOF
	
	
    ) > CMakeLists.txt.template


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


if [ ! -e src/libYARP_OS/src ] ; then
    echo Copying ACE source files
    src=`cat ace_sources.txt | sed "s|ace/||g"`
    mkdir -p src/libYARP_OS/src
    for s in $src; do
	cp ace_orig/$s src/libYARP_OS/src/_ace_$s
    done
fi

if [ ! -e src/libYARP_OS/include/ace ] ; then
    echo Copying ACE header files
    mkdir -p src/libYARP_OS/include/ace
    cp -r ace_orig/* src/libYARP_OS/include/ace
fi


if [ ! -e src/libYARP_OS/include/ace/config.h ] ; then
    echo Creating simple ACE config.h
(
cat <<XXX
#ifndef __ACE4YARP_INC__
#define __ACE4YARP_INC__

#ifndef _REENTRANT
#define _REENTRANT
#endif


// ACE4YARP only has to work on 32-bit Linux and Windows,
// so we can take some shortcuts

#ifdef CYGWIN
#ifndef YARP2_CYGWIN
#define YARP2_CYGWIN
#endif
#endif

#ifdef WIN32
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef _WIN32
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef __WIN__
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef __WINDOWS__
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef WINDOWS
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif


#ifdef __MINGW__
#ifndef YARP2_MINGW
#define YARP2_MINGW
#endif
#endif

#ifdef __MINGW32__
#ifndef YARP2_MINGW
#define YARP2_MINGW
#endif
#endif

#ifdef __LINUX__
#ifndef YARP2_LINUX
#define YARP2_LINUX
#endif
#endif

#ifdef __linux__
#ifndef YARP2_LINUX
#define YARP2_LINUX
#endif
#endif

#ifdef __darwin__
#ifndef YARP2_OSX
#define YARP2_OSX
#endif
#endif

#ifdef __DARWIN__
#ifndef YARP2_OSX
#define YARP2_OSX
#endif
#endif

#ifdef DARWIN
#ifndef YARP2_OSX
#define YARP2_OSX
#endif
#endif

#ifdef MACOSX
#ifndef YARP2_OSX
#define YARP2_OSX
#endif
#endif

#ifdef __APPLE__
#ifndef YARP2_OSX
#define YARP2_OSX
#endif
#endif


#ifdef YARP2_WINDOWS
#ifdef YARP2_MINGW
#ifndef __MINGW32__
#define __MINGW32__
#endif
#define ACE_HAS_EXCEPTIONS  0
#define __ACE_INLINE__ 0
#define ACE_HAS_ACE_TOKEN 0
#define ACE_HAS_ACE_SVCCONF 0
#define ACE_BUILD_DLL 0
#define ACE_HAS_CUSTOM_EXPORT_MACROS 1
#define ACE_HAS_CUSTOM_MACROS 1

// casino in config-win32-common
#define ACE_CONFIG_WIN32_H
#include <ace/config-win32-common.h>
#undef ACE_CONFIG_WIN32_H
#ifdef ACE_HAS_CUSTOM_EXPORT_MACROS
# undef ACE_HAS_CUSTOM_EXPORT_MACROS
# undef ACE_Proper_Export_Flag
# undef ACE_Proper_Import_Flag
# undef ACE_EXPORT_SINGLETON_DECLARATION
# undef ACE_EXPORT_SINGLETON_DECLARE
# undef ACE_IMPORT_SINGLETON_DECLARATION
# undef ACE_IMPORT_SINGLETON_DECLARE
#endif

#endif
#include <ace/config-win32.h>
#ifndef ACE4YARP_CONFIG_OK
#define ACE4YARP_CONFIG_OK
#endif
#endif

#ifdef YARP2_LINUX
#include <ace/config-linux.h>
#ifndef ACE4YARP_CONFIG_OK
#define ACE4YARP_CONFIG_OK
#endif
#endif


#ifndef ACE4YARP_CONFIG_OK
#error "No luck, could not configure ACE4YARP - please compile ACE separately"
#endif

// Disable G++ (>= 4.x) visibility attributes
//#ifndef __G4_ISSUE
//#define __G4_ISSUE
#if (__GNUC__ > 3)
#ifdef ACE_HAS_CUSTOM_EXPORT_MACROS
# undef ACE_HAS_CUSTOM_EXPORT_MACROS
# undef ACE_Proper_Export_Flag
# undef ACE_Proper_Import_Flag
# undef ACE_EXPORT_SINGLETON_DECLARATION
# undef ACE_EXPORT_SINGLETON_DECLARE
# undef ACE_IMPORT_SINGLETON_DECLARATION
# undef ACE_IMPORT_SINGLETON_DECLARE
#endif
#endif  /* __GNU__ > 3 */
//#endif

#endif
XXX
) >> src/libYARP_OS/include/ace/config.h
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
    scp $YACE.zip $YACE.tar.gz eris.liralab.it:/var/www/html/yarp/downloads/yarp && ( echo done > sent-$YACE.txt )
fi
