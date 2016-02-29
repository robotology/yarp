#!/bin/bash

##############################################################################
#
# Copyright: (C) 2011 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
# Download and compile GSL from source.
# 
# Command line arguments: 
#   build_gsl.sh ${OPT_COMPILER} ${OPT_VARIANT} ${OPT_BUILD}
# Example:
#   build_gsl.sh v10 x86 Release
#
# Inputs:
#   settings.sh (general configuration)
#      see process_options.sh for files read based on settings.sh
#   $SETTINGS_BUNDLE_FILENAME
#      read versions of software from file specified in settings.sh
#   cmake_any_any_any.sh
#      cmake paths
#   ace_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
#      ace paths
#   gsl_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
#      gsl paths
#   gtkmm_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
#      gtkmm paths
#
# Outputs:
#   gsl_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
#      gsl paths   
#   gsl-$BUNDLE_GSL_VERSION-$COMPILER_FAMILY
#      directory containing a downloaded version of GSL.
#      this directory is shared by build_gsl.sh calls of the
#      same compiler family (msvc, mingw).  A "cmake" subdirectory 
#      is added to configure GSL for a particular compiler family.
#   gsl-$BUNDLE_GSL_VERSION-$OPT_COMPILER-$OPT_VARIANT-$OPT_BUILD
#      build directory
#   [build directory]/compile_base.sh
#      saves the full environment needed to replicate this build
#   [build directory]/compile.sh
#      tiny script to rerun the build - useful for testing
#   gsl-$BUNDLE_GSL_VERSION-$OPT_COMPILER-$OPT_VARIANT-$OPT_BUILD.zip
#      zip file of build


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
link="http://ftpmirror.gnu.org/gsl/$fname.tar.gz"
if [ ! -e $fname ]; then
	if [ ! -e $fname.tar.gz ]; then
		wget $link || {
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
	find .. -maxdepth 2 -iname "gsl*.h" | sed "s|\.\./||g"
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

# remember where zip is, as we may lose the path when compiling.
zipper=`which zip`

# Set up configure and build steps.  We have to be careful for MINGW.
(
cat << XXX
	source $SETTINGS_SOURCE_DIR/src/restrict_path.sh
	"$CMAKE_BIN" -DGSL_VERSION=$BUNDLE_GSL_VERSION -G "$OPT_GENERATOR" $OPT_CMAKE_OPTION ../$fname1/cmake || exit 1
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


# Request from Lorenzo:
#   Place gsl.lib and gslcblas.lib inside a "lid" directory in the root of the package
#   (i.e. gsl-1.14-v10-x86-Release/lib)

cd $GSL_DIR
mkdir -p lib || exit 1

target_lib_name $OPT_BUILD "gsl" # sets $TARGET_LIB
GSL_LIBRARY="$GSL_DIR/$TARGET_LIB"
cp $GSL_LIBRARY $GSL_DIR/lib || exit 1
GSL_LIBRARY="$GSL_DIR/lib/$TARGET_LIB"

target_lib_name $OPT_BUILD "gslcblas" # sets $TARGET_LIB
GSLCBLAS_LIBRARY="$GSL_DIR/$TARGET_LIB"
cp $GSLCBLAS_LIBRARY $GSL_DIR/lib || exit 1
GSLCBLAS_LIBRARY="$GSL_DIR/lib/$TARGET_LIB"

# Make a GSL ZIP file for Lorenzo
cd $GSL_DIR
(
	echo "GSL version $BUNDLE_GSL_VERSION"
	echo "Downloaded from $link"
	date
	echo " "
	echo "Compiler family: $COMPILER_FAMILY"
	echo "Compiler version: $OPT_COMPILER $OPT_VARIANT $OPT_BUILD"
) > BUILD_INFO.TXT
cd ..
rm -f $fname2.zip
$zipper -r $fname2.zip $fname2/BUILD_INFO.TXT $fname2/include $fname2/lib || exit 1

# Cache GSL-related paths and variables, for dependent packages to read
(
	echo "export GSL_DIR='$GSL_DIR'"
	echo "export GSL_ROOT='$GSL_ROOT'"
	echo "export GSL_LIBRARY='$GSL_LIBRARY'"
	echo "export GSLCBLAS_LIBRARY='$GSLCBLAS_LIBRARY'"
	echo "export GSL_INCLUDE_DIR='$GSL_DIR/include/'"
) > $BUILD_DIR/gsl_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
