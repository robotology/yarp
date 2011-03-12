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

source cmake_${compiler}_${variant}_${build}.sh || {
	echo "Cannot find corresponding CMAKE build"
	exit 1
}

if [ "k$GSL_VERSION" = "k" ]; then
	GSL_VERSION=1.14
fi

fname=gsl-$GSL_VERSION
if [ ! -e $fname ]; then
	if [ ! -e $fname.tar.gz ]; then
		wget ftp://gnu.mirrors.pair.com/gnu/gnu/gsl/$fname.tar.gz || {
			echo "Cannot fetch GSL"
			exit 1
		}
	fi
	tar xzvf $fname.tar.gz || {
		echo "Cannot unpack GSL"
		exit 1
	}
fi

mkdir -p $fname/cmake
cd $fname/cmake || exit 1
if [ ! -e Headers.cmake ] ; then
  # generate list of parts as Parts.cmake
  find .. -mindepth 2 -iname "Makefile.am" -exec grep -H "_la_SOURCES" {} \; | sed "s|.*/\([-a-z]*\)/Makefile.am|\1 |" | sed "s|:.*=||" | sed "s|^|ADD_PART(|" | sed "s|$|)|" | tee Parts.cmake
  # generate list of headers as Headers.cmake
  (
	echo "set(GSL_HEADERS"
	echo -n "  "
	find .. -iname "gsl*.h" -maxdepth 2 | sed "s|\.\./||g"
	echo ")"
	) | tee Headers.cmake
fi
cp "$SOURCE_DIR/src/gsl/CMakeLists.txt" "$PWD"
cp "$SOURCE_DIR/src/gsl/config.h.in" "$PWD"
cd $BUILD_DIR

fname2=$fname-$compiler-$variant-$build

mkdir -p $fname2
cd $fname2 || exit 1

"$CMAKE_BIN" -DGSL_VERSION=$GSL_VERSION -G "$generator" ../$fname/cmake || exit 1
$BUILDER gsl.sln $CONFIGURATION_COMMAND $PLATFORM_COMMAND

(
	GSL_DIR=`cygpath --mixed "$PWD"`
	GSL_ROOT=`cygpath --mixed "$PWD/../$fname"`
	echo "export GSL_DIR='$GSL_DIR'"
	echo "export GSL_ROOT='$GSL_ROOT'"
	echo "export GSL_LIBRARY='$GSL_DIR/$build/gsl.lib'"
	echo "export GSLCBLAS_LIBRARY='$GSL_DIR/$build/gslcblas.lib'"
	echo "export GSL_INCLUDE_DIR='$GSL_DIR/include/'"
) > $BUILD_DIR/gsl_${compiler}_${variant}_${build}.sh
