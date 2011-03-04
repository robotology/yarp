#!/bin/bash

# Actually, we just download a precompiled gtkmm.
# We could compile it, but it doesn't seem like we need to do better distributing it
# than what already exists.

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

if [ "k$CMAKE_VERSION" = "k" ]; then
	CMAKE_VERSION=2.8.4
fi

fname=cmake-$CMAKE_VERSION

if [ ! -e $fname.zip ]; then
	CMAKE_DIR=`echo $CMAKE_VERSION | sed "s/\.[-0-9]*$//"`
	zipname="http://www.cmake.org/files/v$CMAKE_DIR/cmake-$CMAKE_VERSION-win32-x86.zip"
	wget -O $fname.zip $zipname || (
		echo "Cannot fetch CMAKE"
		rm -f $fname.zip
		exit 1
	)
fi

if [ ! -d $fname ]; then
	mkdir -p $fname
	cd $fname
	unzip ../$fname.zip || {
		echo "Cannot unpack CMAKE"
		cd $BUILD_DIR
		rm -rf $fname
		exit 1
	}
fi

cd $BUILD_DIR

(
	CMAKE_DIR=`cygpath --mixed $PWD/$fname/cmake-*`
	echo "export CMAKE_DIR='$CMAKE_DIR'"
	echo "export CMAKE_BIN='$CMAKE_DIR/bin/cmake.exe'"
) > $BUILD_DIR/cmake_${compiler}_${variant}_${build}.sh
