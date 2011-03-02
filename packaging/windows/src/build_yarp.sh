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

fname=yarp-$YARP_VERSION

if [ ! -e $fname ]; then
	if [ "k$YARP_VERSION" = "ktrunk" ]; then
		svn co https://yarp0.svn.sourceforge.net/svnroot/yarp0/trunk/yarp2 $fname || {
			echo "Cannot fetch YARP"
			exit 1
		}
	else
		svn co https://yarp0.svn.sourceforge.net/svnroot/yarp0/tags/$fname $fname || {
			echo "Cannot fetch YARP"
			exit 1
		}
	fi
fi

fname2=$fname-$variant-$build

mkdir -p $fname2
cd $fname2 || exit 1

echo "Using ACE from $ACE_ROOT"
echo "Using GSL from $GSL_DIR"
"$CMAKE_EXEC" -DCREATE_LIB_MATH=TRUE -DGSL_LIBRARY="$GSL_LIBRARY" -DGSLCBLAS_LIBRARY="$GSLCBLAS_LIBRARY" -DGSL_DIR="$GSL_DIR" -DCREATE_SHARED_LIBRARY=TRUE -DYARP_COMPILE_TESTS=TRUE -DYARP_FILTER_API=TRUE -G "$generator" ../$fname || exit 1
$BUILDER YARP.sln $CONFIGURATION_COMMAND $PLATFORM_COMMAND

(
	YARP_DIR=`cygpath --mixed "$PWD"`
	YARP_ROOT=`cygpath --mixed "$PWD/../$fname"`
	echo "export YARP_DIR='$YARP_DIR'"
	echo "export YARP_ROOT='$YARP_ROOT'"
) > $BUILD_DIR/yarp_${compiler}_${variant}_${build}.sh
