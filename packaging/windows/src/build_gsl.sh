#!/bin/bash

##############################################################################
#
# Copyright: (C) 2011 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
# Download and compile GSL from source.
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

# Pick up CMake configuration
source cmake_any_any_any.sh || {
	echo "Cannot find corresponding CMAKE build"
	exit 1
}

# Check GSL version is set
if [ "k$BUNDLE_GSL_VERSION" = "k" ]; then
	echo "Set BUNDLE_GSL_VERSION"
	exit 1
fi

# Go ahead and download GSL source code
fname=gsl-$BUNDLE_GSL_VERSION
if [ ! -e $fname ]; then
	if [ ! -e $fname.tar.gz ]; then
		wget ftp://gnu.mirrors.pair.com/gnu/gnu/gsl/$fname.tar.gz || {
			echo "Cannot fetch GSL"
			exit 1
		}
	fi
fi

# Unpack source code
fname1=$fname-$COMPILER_FAMILY
if [ ! -e $fname1 ]; then
	tar xzvf $fname.tar.gz || {
		echo "Cannot unpack GSL"
		exit 1
	}
	mv $fname $fname1 || exit 1
fi

# Add CMake material for building - easiest way to deal with Windows.
mkdir -p $fname1/cmake
cd $fname1/cmake || exit 1
if [ ! -e Headers.cmake ] ; then
  # Generate list of parts as Parts.cmake
  find .. -mindepth 2 -iname "Makefile.am" -exec grep -H "_la_SOURCES" {} \; | sed "s|.*/\([-a-z]*\)/Makefile.am|\1 |" | sed "s|:.*=||" | sed "s|^|ADD_PART(|" | sed "s|$|)|" | tee Parts.cmake
  # Generate list of headers as Headers.cmake
  (
	echo "set(GSL_HEADERS"
	echo -n "  "
	find .. -iname "gsl*.h" -maxdepth 2 | sed "s|\.\./||g"
	echo ")"
	) | tee Headers.cmake
fi
cp "$SETTINGS_SOURCE_DIR/src/gsl/CMakeLists.txt" "$PWD"
cp "$SETTINGS_SOURCE_DIR/src/gsl/${COMPILER_FAMILY}_config.h.in" "$PWD/config.h.in" || exit 1
cd $BUILD_DIR

# Make and enter build directory
fname2=$fname-$OPT_COMPILER-$OPT_VARIANT-$OPT_BUILD
mkdir -p $fname2
cd $fname2 || exit 1

# Set up configure and build steps.  We have to be careful for MINGW.
(
cat << XXX
	source $SETTINGS_SOURCE_DIR/src/restrict_path.sh
	"$CMAKE_BIN" -DBUNDLE_GSL_VERSION=$BUNDLE_GSL_VERSION -G "$OPT_GENERATOR" $OPT_CMAKE_OPTION ../$fname1/cmake || exit 1
	target_name "gsl"
	$OPT_BUILDER \$user_target \$TARGET $OPT_CONFIGURATION_COMMAND $OPT_PLATFORM_COMMAND
XXX
) > compile_base.sh
# Make a small compile script for user testing
(
	set
	echo 'user_target="$1"'
	echo "source compile_base.sh"
) > compile.sh

GSL_DIR=`cygpath --mixed "$PWD"`
GSL_ROOT=`cygpath --mixed "$PWD/../$fname"`

# Configure and build
{
	source compile_base.sh || exit 1
}

# Cache GSL-related paths and variables, for dependent packages to read
(
	echo "export GSL_DIR='$GSL_DIR'"
	echo "export GSL_ROOT='$GSL_ROOT'"
	target_lib_name $OPT_BUILD "gsl" # sets $TARGET_LIB
	echo "export GSL_LIBRARY='$GSL_DIR/$TARGET_LIB'"
	target_lib_name $OPT_BUILD "gslcblas" # sets $TARGET_LIB
	echo "export GSLCBLAS_LIBRARY='$GSL_DIR/$TARGET_LIB'"
	echo "export GSL_INCLUDE_DIR='$GSL_DIR/include/'"
) > $BUILD_DIR/gsl_${compiler}_${variant}_${build}.sh
