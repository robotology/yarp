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

source yarp_${compiler}_${variant}_${build}.sh || {
	echo "Cannot find corresponding YARP build"
	exit 1
}

if [ "k$compiler" = "kv10" ] ; then
	platform=v100
else 
	echo "Please set platform for compiler $compiler in build_icub.sh"
	exit 1
fi


BUILD_DIR=$PWD

PLATFORM_COMMAND="/p:PlatformToolset=$platform"
CONFIGURATION_COMMAND="/p:Configuration=$build"

if [ "k$ICUB_VERSION" = "k" ]; then
	ICUB_VERSION=trunk
fi

fname=icub-$ICUB_VERSION

if [ ! -e $fname ]; then
	if [ "k$ICUB_VERSION" = "ktrunk" ]; then
		svn co https://robotcub.svn.sourceforge.net/svnroot/robotcub/trunk/iCub/main $fname || {
			echo "Cannot fetch ICUB"
			exit 1
		}
	else
		echo "ICUB versioned check-out not set up yet"
		exit 1
	fi
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
	echo "Please set generator for compiler $compiler variant $variant in build_icub.sh"
	exit 1
fi

echo "Using ACE from $ACE_ROOT"
echo "Using YARP from $YARP_DIR"
"$CMAKE_EXEC" -G "$generator" ../$fname || exit 1
msbuild.exe ICUB.sln $CONFIGURATION_COMMAND $PLATFORM_COMMAND
