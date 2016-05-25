#!/bin/bash
#set -x
##############################################################################
#
# Copyright: (C) 2011 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
# Download and build YARP from source
#
# Command line arguments: 
#   build_yarp.sh ${OPT_COMPILER} ${OPT_VARIANT} ${OPT_BUILD}
# Example:
#   build_yarp.sh v10 x86 Release
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
#   yarp_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
#      yarp paths   
#   yarp-$BUNDLE_YARP_VERSION
#      directory containing a checked out version of yarp.
#      this directory is shared by a build_yarp.sh calls, regardless
#      of arguments supplied.
#   yarp-$BUNDLE_YARP_VERSION-$OPT_COMPILER-$OPT_VARIANT-$OPT_BUILD
#      build directory
#   [build directory]/compile_base.sh
#      saves the full environment needed to replicate this build
#   [build directory]/compile.sh
#      tiny script to rerun the build - useful for testing

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

# Pick up CMake paths
source cmake_any_any_any.sh || {
	echo "Cannot find corresponding CMAKE build"
	exit 1
}

# Pick up ACE paths
source ace_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh || {
	echo "Cannot find corresponding ACE build"
	exit 1
}

# Pick up GSL paths
source gsl_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh || {
	echo "Cannot find corresponding GSL build"
	exit 1
}

# Pick up GTKMM paths
if [ -f "gtkmm_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh" ]; then
  source gtkmm_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh 
else
	echo "No GTKMM build"
fi

# Pick up QT paths
if [ -f "qt_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh" ]; then
  source qt_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
else
    echo "NO QT build"
fi

# Go ahead and download YARP
echo "BUNDLE_YARP_VERSION: $BUNDLE_YARP_VERSION"
source_name="yarp"
source_url="https://github.com/robotology/${source_name}" 

if [ ! -e "$source_name" ]; then
  git clone $source_url || {
    echo "Cannot fetch ${source_name} from $source_url"
    exit 1
  }
fi
cd $source_name
if [ "$BUNDLE_YARP_VERSION" == "" ] || [ "$BUNDLE_YARP_VERSION" == "trunk" ] || [ "$BUNDLE_YARP_VERSION" == "master" ]
then
  git checkout master || {
    echo "Cannot fetch YARP trunk"
    exit 1
  }
  git  pull || {
    echo "Cannot update $source_name from $source_url"
    exit 1
  }
else
  git checkout  v${BUNDLE_YARP_VERSION} || {
    echo "Cannot fetch YARP v${BUNDLE_YARP_VERSION}"
    exit 1
  }
fi
cd ..
# Make and enter build directory
build_path="${source_name}-${OPT_COMPILER}-${OPT_VARIANT}-${OPT_BUILD}"
echo "Building to ${build_path}"
if [ -d "$build_path" ]; then
  rm -rf $build_path
fi
mkdir -p $build_path
cd $build_path

YARP_DIR=`cygpath --mixed "$PWD"`
YARP_ROOT=`cygpath --mixed "$PWD/../${source_name}"`

echo "Using ACE from $ACE_ROOT"
echo "Using GSL from $GSL_DIR"

# Set up configure and build steps.  We have to be careful for MINGW.
(
cat << XXX
	source $SETTINGS_SOURCE_DIR/src/restrict_path.sh

	"$CMAKE_BIN" -G "$OPT_GENERATOR" $OPT_CMAKE_OPTION $YARP_CMAKE_OPTIONS -DCMAKE_INSTALL_PREFIX=$YARP_DIR/install -DGSL_DIR="$GSL_DIR" -DGTK_BASEPATH="$GTK_BASEPATH" ../$source_name || exit 1
	target_name "YARP"
	$OPT_BUILDER  \$user_target \$TARGET $OPT_CONFIGURATION_COMMAND $OPT_PLATFORM_COMMAND || exit 1
	# if [ ! -e install ]; then
	"$CMAKE_BIN" --build . --target install --config ${OPT_BUILD} || exit 1
	# fi
XXX
) > compile_base.sh
# Make a small compile script for user testing
(
	set
	echo 'user_target="$1"'
	echo "source compile_base.sh"
) > compile.sh

# Configure and build
{
	source compile_base.sh || exit 1
}

# Cache YARP-related paths and variables, for dependent packages to read
(
	echo "export YARP_DIR='$YARP_DIR'"
	echo "export YARP_ROOT='$YARP_ROOT'"
) > $BUILD_DIR/yarp_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
