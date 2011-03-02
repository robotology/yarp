#!/bin/bash

BUILD_DIR=$PWD

source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

source $BUNDLE_FILENAME || {
	echo "Bundle settings not found"
	exit 1
}

source $SOURCE_DIR/src/process_options.sh $* || {
	echo "Cannot process options"
	exit 1
}

source ace_${compiler}_${variant}_${build}.sh || {
	echo "Cannot find corresponding ACE build"
	exit 1
}

source gsl_${compiler}_${variant}_${build}.sh || {
	echo "Cannot find corresponding GSL build"
	exit 1
}

source yarp_${compiler}_${variant}_${build}.sh || {
	echo "Cannot find corresponding YARP build"
	exit 1
}

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

mkdir -p $fname2
cd $fname2 || exit 1

echo "Using ACE from $ACE_ROOT"
echo "Using YARP from $YARP_DIR"
"$CMAKE_EXEC" -DGSL_LIBRARY="$GSL_LIBRARY" -DGSLCBLAS_LIBRARY="$GSLCBLAS_LIBRARY" -DGSL_DIR="$GSL_DIR" -G "$generator" ../$fname || exit 1
$BUILDER ICUB.sln $CONFIGURATION_COMMAND $PLATFORM_COMMAND
