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

source ace_${compiler}_${variant}_${build}.sh || {
	echo "Cannot find corresponding ACE build"
	exit 1
}

if [ "k$compiler" = "kv10" ] ; then
	platform=v100
else 
	echo "Please set platform for compiler $compiler in build_yarp.sh"
	exit 1
fi


BUILD_DIR=$PWD

PLATFORM_COMMAND="/p:PlatformToolset=$platform"
CONFIGURATION_COMMAND="/p:Configuration=$build"

fname=yarp-$YARP_VERSION

if [ ! -e $fname ]; then
	svn co https://yarp0.svn.sourceforge.net/svnroot/yarp0/tags/$fname $fname || {
		echo "Cannot fetch YARP"
		exit 1
	}
fi

fname2=$fname-$variant-$build

if [ ! -e $fname2 ]; then
	mkdir -p $fname2
fi

cd $fname2 || exit 1

generator=""
if [ "k$compiler" = "kv10" ] ; then
	if [ "k$variant" = "kx86" ] ; then
		generator="Visual Studio 10"
	fi
fi
if [ "k$generator" = "k" ] ; then 
	echo "Please set generator for compiler $compiler variant $variant in build_yarp.sh"
	exit 1
fi

echo "Using ACE from $ACE_ROOT"
"$CMAKE_EXEC" -G "$generator" ../$fname || exit 1
msbuild.exe YARP.sln $CONFIGURATION_COMMAND $PLATFORM_COMMAND
