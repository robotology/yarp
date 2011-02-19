#!/bin/bash

source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

source $BUNDLE_FILENAME || {
	echo "Bundle settings not found"
	exit 1
}

compiler=$1
variant=$2
build=$3

source compiler_config_${compiler}_${variant}.sh || {
	echo "Compiler settings not found"
	exit 1
}

BUILD_DIR=$PWD

fname=ACE-$ACE_VERSION

if [ "k$compiler" = "kv10" ] ; then
	pname=ACE_vc10.vcxproj
	platform=v100
else 
	echo "Please set project name for compiler $compiler in build_ace.sh"
	exit 1
fi

PLATFORM_COMMAND="/p:PlatformToolset=$platform"
CONFIGURATION_COMMAND="/p:Configuration=$build"

if [ ! -e $fname.tar.gz ]; then
	wget http://download.dre.vanderbilt.edu/previous_versions/$fname.tar.gz || (
		echo "Cannot fetch ACE"
		exit 1
	)
fi

# msbuild does not have out-of-source builds, so we duplicate source as needed
fname2=$fname-$variant

if [ ! -e $fname2 ]; then
	tar xzvf $fname.tar.gz || (
		echo "Cannot unpack ACE"
		exit 1
	)
	mv ACE_wrappers $fname2 || exit 1
fi

cd $fname2 || exit 1
export ACE_ROOT=$PWD
if [ ! -e $ACE_ROOT/ace/config.h ] ; then
	echo "Creating $ACE_ROOT/ace/config.h"	
	cd $ACE_ROOT/ace
	echo '#include "ace/config-win32.h"' > config.h
fi

cd $ACE_ROOT/ace
echo $PWD
if [ ! -e $pname ]; then
	echo "Could not find $pname"
	exit 1
fi

echo msbuild.exe $pname $CONFIGURATION_COMMAND $PLATFORM_COMMAND
msbuild.exe $pname $CONFIGURATION_COMMAND $PLATFORM_COMMAND || exit 1

(
	ACE_DIR=`cygpath --mixed "$ACE_ROOT"`
	echo "export ACE_DIR='$ACE_DIR'"
	echo "export ACE_ROOT='$ACE_DIR'"
) > $BUILD_DIR/ace_${compiler}_${variant}_${build}.sh
