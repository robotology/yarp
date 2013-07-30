#!/bin/bash

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
source gtkmm_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh || {
	echo "Cannot find corresponding GTKMM build"
	exit 1
}

# Go ahead and download YARP
echo "BUNDLE_YARP_VERSION: $BUNDLE_YARP_VERSION"
fname="yarp"

if [ ! -e "$fname" ]; then
	if [ "$BUNDLE_YARP_VERSION" == "trunk" ]; then
			svn co https://github.com/robotology/yarp/trunk $fname || {
				echo "Cannot fetch YARP"
				exit 1
		}
	else
		if [ "$BUNDLE_YARP_REVISION" == "" ]; then
				fname="${fname}-${BUNDLE_YARP_VERSION}"
				svn co https://github.com/robotology/yarp/tags/v${BUNDLE_YARP_VERSION} $fname || {
					echo "Cannot fetch YARP"
					exit 1
			}
		else
			FNAME="${fname}-r${BUNDLE_YARP_REVISION}"
			svn co https://github.com/robotology/yarp/trunk -r $BUNDLE_YARP_REVISION $fname || {
				echo "Cannot fetch YARP"
				exit 1
			}
		fi
	fi
fi

# Make and enter build directory
fname2=${fname}-${OPT_COMPILER}-${OPT_VARIANT}-${OPT_BUILD}
mkdir -p $fname2
cd $fname2 || exit 1

YARP_DIR=`cygpath --mixed "$PWD"`
YARP_ROOT=`cygpath --mixed "$PWD/../${fname}"`

echo "Using ACE from $ACE_ROOT"
echo "Using GSL from $GSL_DIR"

# Set up configure and build steps.  We have to be careful for MINGW.
(
cat << XXX
	source $SETTINGS_SOURCE_DIR/src/restrict_path.sh

	# Old CMake setup, prior to reorganizing GSL, GTKMM for Lorenzo
	#	"$CMAKE_BIN" $OPT_CMAKE_OPTION -DCMAKE_INSTALL_PREFIX=$YARP_DIR/install -DCREATE_YARPSERVER3=TRUE -DCREATE_LIB_MATH=TRUE -DCMAKE_LIBRARY_PATH="$GTKMM_BASEPATH/include" -DCMAKE_INCLUDE_PATH="$GTKMM_BASEPATH/include/include" -DFREETYPE_INCLUDE_DIRS="." -DFREETYPE_LIBRARIES="" -DFREETYPE_LIBRARY="" -DGSL_LIBRARY="$GSL_LIBRARY" -DGSLCBLAS_LIBRARY="$GSLCBLAS_LIBRARY" -DGSL_DIR="$GSL_DIR" -DCREATE_GUIS=TRUE -DCREATE_SHARED_LIBRARY=TRUE -DYARP_COMPILE_TESTS=TRUE -DYARP_FILTER_API=TRUE -G "$OPT_GENERATOR" ../$fname || exit 1

	# Post-GSL reorganization
	#"$CMAKE_BIN" $OPT_CMAKE_OPTION -DCMAKE_INSTALL_PREFIX=$YARP_DIR/install -DCREATE_YARPSERVER3=TRUE -DCREATE_LIB_MATH=TRUE -DCMAKE_LIBRARY_PATH="$GTKMM_BASEPATH/include" -DCMAKE_INCLUDE_PATH="$GTKMM_BASEPATH/include/include" -DFREETYPE_INCLUDE_DIRS="." -DFREETYPE_LIBRARIES="" -DFREETYPE_LIBRARY="" -DGSL_DIR="$GSL_DIR" -DCREATE_GUIS=TRUE -DCREATE_SHARED_LIBRARY=TRUE -DYARP_COMPILE_TESTS=TRUE -DYARP_FILTER_API=TRUE -G "$OPT_GENERATOR" ../$fname || exit 1
	
	# Post-GSL reorganization, Post-GTKMM reorganization
	"$CMAKE_BIN" $OPT_CMAKE_OPTION -DCMAKE_INSTALL_PREFIX=$YARP_DIR/install -DCREATE_YMANAGER=TRUE -DCREATE_YMANAGER_GUI=TRUE -DCREATE_LIB_MATH=TRUE -DGSL_DIR="$GSL_DIR" -DGTK_BASEPATH="$GTK_BASEPATH" -DCREATE_GUIS=TRUE -DCREATE_SHARED_LIBRARY=TRUE -DYARP_COMPILE_TESTS=TRUE -DYARP_FILTER_API=TRUE -DCREATE_IDLS=TRUE -DENABLE_yarpidl_thrift=TRUE -DCREATE_OPTIONAL_CARRIERS=TRUE -DENABLE_yarpcar_tcpros_carrier=TRUE -DENABLE_yarpcar_xmlrpc_carrier=TRUE -DENABLE_yarpcar_bayer_carrier=TRUE -DUSE_LIBDC1394=FALSE -DENABLE_yarpcar_priority_carrier=TRUE -G "$OPT_GENERATOR" ../$fname || exit 1
	
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
