#!/bin/bash

# run as ./scripts/embed-ace.sh

# This script has become a way of compiling a standalone version
# of yarp, for linux.
# We engulf both ACE and libstdc++.

yarp=`ls yarp-*.tar.gz | head -n1`
ace=`ls ace4yarp-*.tar.gz | head -n1`
stdc=`ls libstdc*.a | head -n1`
base=$PWD

if [ "k$yarp" = "k" ]; then
    echo "Need a yarp-*.tar.gz file in the current directory."
    echo "This can be downloaded from http://yarp0.sf.net"
    exit 1
fi

if [ "k$yarp" = "k" ]; then
    echo "Need an ace4yarp-*.tar.gz file in the current directory."
    echo "This can be built by ./scripts/ace-helper.sh"
    exit 1
fi

if [ "k$stdc" = "k" ]; then
    echo "Need a libstdc*.a file in the current directory."
    echo "On debian, this can be found in /usr/lib/gcc/..."
    exit 1
fi

echo Merging $yarp and $ace

rm -rf embed-ace
mkdir embed-ace

cd embed-ace
tar xzvf - < ../$yarp
tar xzvf - < ../$ace

# rationalize names of directories
mv ace4yarp-* ace4yarp
merged=`ls -d yarp-* | sed "s/yarp/yarpsolo/"`
mv yarp-* $merged

# extract ace source cpp files
srcs=`cat ace4yarp/CMakeLists.txt | grep ADD_LIBRARY | sed "s/.*ACE4YARP//" | sed "s|..folder_header.*||"`
echo "Sources: $srcs"

# merge in source and header files
osdir="$merged/src/libYARP_OS"
cd ace4yarp
mkdir -p ../$osdir/src/ace
for src in `echo $srcs | sed "s|ace/||g"`; do
    mv ace/$src ../$osdir/src/_ace_$src
done
cp ace/config-linux.h ace/config.h
mv ace ../$osdir/include

# nobble the conf/FindAce.cmake script
cd ..
cd $merged
cd conf
(
cat<<XXX
SET(ACE_INCLUDE_DIR "/usr/include")
SET(ACE_LIBRARY "-lm -lpthread -ldl")
SET(ACE_LINK_FLAGS "$ACE_LIBRARY")
SET(ACE_FOUND TRUE)
XXX
) > FindAce.cmake

cd ..
(
cat <<XXX
// Disable G++ (>= 4.x) visibility attributes
#ifndef __G4_ISSUE
#define __G4_ISSUE
#if (__GNUC__ > 3)
# undef ACE_HAS_CUSTOM_EXPORT_MACROS
# undef ACE_Proper_Export_Flag
# undef ACE_Proper_Import_Flag
# undef ACE_EXPORT_SINGLETON_DECLARATION
# undef ACE_EXPORT_SINGLETON_DECLARE
# undef ACE_IMPORT_SINGLETON_DECLARATION
# undef ACE_IMPORT_SINGLETON_DECLARE
#endif  /* __GNU__ > 3 */
#endif
XXX
) >> src/libYARP_OS/include/ace/config.h

(
cat <<EOF
PROJECT(YARP)
IF( \${CMAKE_COMPILER_IS_GNUCXX} )
  LINK_DIRECTORIES("\${PROJECT_SOURCE_DIR}/add")
  ADD_DEFINITIONS(-static-libgcc)
  SET(CMAKE_CXX_LINK_EXECUTABLE "\${CMAKE_CXX_LINK_EXECUTABLE} -static-libgcc")
ENDIF( \${CMAKE_COMPILER_IS_GNUCXX} )
EOF
cat CMakeLists.txt | grep -v "PROJECT.YARP"
) > CMakeLists.txt.fix
mv CMakeLists.txt.fix CMakeLists.txt
mkdir add
cp $base/$stdc add

echo "Should be able to run cmake in embed-ace/$merged"

#cd yarp-*/src
#mv ../../ace4yarp-* libYARP_ACE
#
## add the ACE4YARP target
#echo "SUBDIRS(libYARP_ACE)" >> CMakeLists.txt
#cd libYARP_ACE
#
## fix up a path issue in ACE4YARP
#cat CMakeLists.txt | sed "s/CMAKE_SOURCE_DIR/PROJECT_SOURCE_DIR/g" > CMakeLists.txt.fix
#mv CMakeLists.txt.fix CMakeLists.txt
#cd ../..

