#!/bin/bash

# run as ./scripts/ace-helper.sh

VER=5.6.1
LOCALVER=4

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

if [ ! -e src/libYARP_OS/ace_src ] ; then
    echo Copying ACE source files
    src=`cat ace_sources.txt | sed "s|ace/||g"`
    mkdir -p src/libYARP_OS/ace_src
    for s in $src; do
	cp ace_orig/$s src/libYARP_OS/ace_src/_ace_$s
    done
fi

if [ ! -e src/libYARP_OS/ace_include/ace ] ; then
    echo Copying ACE header files
    mkdir -p src/libYARP_OS/ace_include/ace
    cp -r ace_orig/* src/libYARP_OS/ace_include/ace
fi


if [ ! -e src/libYARP_OS/ace_include/ace/config.h ] ; then
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
) >> src/libYARP_OS/ace_include/ace/config.h
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
