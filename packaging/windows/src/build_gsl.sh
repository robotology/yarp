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

if [ "k$compiler" = "kv10" ] ; then
	platform=v100
else 
	echo "Please set platform for compiler $compiler in build_gsl.sh"
	exit 1
fi


BUILD_DIR=$PWD

PLATFORM_COMMAND="/p:PlatformToolset=$platform"
CONFIGURATION_COMMAND="/p:Configuration=$build"

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
  # generate list of headers ad Headers.cmake
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
	echo "Please set generator for compiler $compiler variant $variant in build_gsl.sh"
	exit 1
fi

"$CMAKE_EXEC" -DGSL_VERSION=$GSL_VERSION -G "$generator" ../$fname/cmake || exit 1
msbuild.exe Project.sln $CONFIGURATION_COMMAND $PLATFORM_COMMAND

