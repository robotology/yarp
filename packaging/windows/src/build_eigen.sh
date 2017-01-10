#!/bin/bash

##############################################################################
#
# Copyright: (C) 2011 RobotCub Consortium
# Authors: Matteo Brunettini Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
# Download EIGEN library from source.
# 
# Command line arguments: 
#   build_eigen.sh ${OPT_COMPILER} ${OPT_VARIANT} ${OPT_BUILD}
# Example:
#   build_eigen.sh v10 x86 Release
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
#   eigen_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
#      eigen paths
#   gtkmm_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
#      gtkmm paths
#
# Outputs:
#   eigen_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
#      eigen paths   
#   eigen-$BUNDLE_EIGEN_VERSION-$COMPILER_FAMILY
#      directory containing a downloaded version of EIGEN.
#      this directory is shared by build_eigen.sh calls of the
#      same compiler family (msvc, mingw).  A "cmake" subdirectory 
#      is added to configure EIGEN for a particular compiler family.
#   eigen-$BUNDLE_EIGEN_VERSION-$OPT_COMPILER-$OPT_VARIANT-$OPT_BUILD
#      build directory
#   [build directory]/compile_base.sh
#      saves the full environment needed to replicate this build
#   [build directory]/compile.sh
#      tiny script to rerun the build - useful for testing
#   eigen-$BUNDLE_EIGEN_VERSION-$OPT_COMPILER-$OPT_VARIANT-$OPT_BUILD.zip
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

# Check EIGEN version is set
if [ "$BUNDLE_EIGEN_VERSION" == "" ]; then
  echo "Set BUNDLE_EIGEN_VERSION"
  exit 1
fi

# Go ahead and download EIGEN source code
fname="eigen-${BUNDLE_EIGEN_VERSION}"
link="${BUNDLE_EIGEN_URL}/${BUNDLE_EIGEN_VERSION}.tar.gz"
if [ ! -e "$fname" ]; then
  if [ ! -e "${fname}.tar.gz" ]; then
    wget $link --output-document="$fname.tar.gz" || {
      echo "Cannot fetch EIGEN from $link"
      exit 1
    }
  fi
  mkdir $fname && tar xzvf ${fname}.tar.gz -C $fname --strip-components 1 || {
    echo "Cannot unpack EIGEN"
    exit 1
  }
fi

# Cache EIGEN-related paths and variables, for dependent packages to read
(
  EIGEN_DIR=$(cygpath --mixed "${BUILD_DIR}/${fname}")
  echo "export EIGEN_DIR='$EIGEN_DIR'"
  echo "export EIGEN3_ROOT='$EIGEN_RIR'"
) > $BUILD_DIR/eigen_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
