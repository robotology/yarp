#!/bin/bash

##############################################################################
#
# Copyright: (C) 2011 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
# Download a precompiled version of cmake.
# 

BUILD_DIR=$PWD

# Get SETTINGS_* variables (paths) from cache
source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

# Get BUNDLE_* variables (software versions) from the bundle file
source $SETTINGS_BUNDLE_FILENAME || {
	echo "Bundle settings not found"
	exit 1
}

# GET OPT_* variables (build options) by processing our command-line options
source $SETTINGS_SOURCE_DIR/src/process_options.sh $* || {
	echo "Cannot process options"
	exit 1
}

if [ "k$BUNDLE_CMAKE_VERSION" = "k" ]; then
	echo "set BUNDLE_CMAKE_VERSION"
	exit 1
fi

# Go ahead and download
fname=cmake-$BUNDLE_CMAKE_VERSION
if [ ! -e $fname.zip ]; then
	CMAKE_DIR=`echo $BUNDLE_CMAKE_VERSION | sed "s/\.[-0-9]*$//"`
	zipname="http://www.cmake.org/files/v$CMAKE_DIR/cmake-$BUNDLE_CMAKE_VERSION-win32-x86.zip"
	wget -O $fname.zip $zipname || (
		echo "Cannot fetch CMAKE"
		rm -f $fname.zip
		exit 1
	)
fi

# Unpack if needed
if [ ! -d $fname ]; then
	mkdir -p $fname
	cd $fname
	unzip ../$fname.zip || {
		echo "Cannot unpack CMAKE"
		cd $BUILD_DIR
		rm -rf $fname
		exit 1
	}
	find . -name *.exe -exec chmod +x '{}' \;
	find . -name *.dll -exec chmod +x '{}' \;
fi


# Cache CMake-related paths and variables, for dependent packages to read
cd $BUILD_DIR
(
	CMAKE_DIR=`cygpath --mixed $PWD/$fname/cmake-*`
	echo "export CMAKE_DIR='$CMAKE_DIR'"
	echo "export CMAKE_BIN='$CMAKE_DIR/bin/cmake.exe'"
) > $BUILD_DIR/cmake_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
