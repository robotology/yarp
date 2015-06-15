#!/bin/bash

##############################################################################
#
# Copyright: (C) 2011 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
# Test if YARP .deb file works, in a fresh chroot.
#
# Command line arguments: 
#   test_yarp.sh ${OPT_PLATFORM}
# Example:
#   test_yarp.sh etch
#
# Inputs:
#   settings.sh (general configuration)
#   chroot_${OPT_PLATFORM}.sh
#      records name and directory of a basic chroot
#   yarp_${OPT_PLATFORM}.sh
#      records location of .deb file
#
# Outputs:
#   ${OPT_CHROOT_DIRECTORY}/test_chroot
#      a chroot within which YARP has been tested

BUILD_DIR=$PWD

source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

source $BUNDLE_FILENAME || {
	echo "No bundle settings found: Tried '$BUNDLE_FILENAME'"
	exit 1
}

platform=$1

if [ "k$platform" = "k" ]; then
    echo "Call as: test_yarp.sh <platform>"
    exit 1
fi

# Load the configuration of the desired platform
source ./config_$platform.sh || {
	echo "No platform configuration file found"
	exit 1
}

# Load the configuration of a clean chroot
source chroot_${platform}.sh || {
	echo "Cannot find corresponding chroot"
	exit 1
}

# Load the configuration of the chroot used for building
source yarp_${platform}.sh || {
	echo "Cannot find corresponding YARP package"
	exit 1
}

# Helper for running a command within the test chroot
function run_in_chroot {
    echo "Running [$2]"
    sudo chroot $1 bash -c "$2"
}

# Make sure YARP .deb is present
cd $YARP_PACKAGE_DIR
test -e $YARP_PACKAGE || exit 1

# Create a chroot for testing, if one does not already exist
if [ ! -e test_chroot ]; then
    sudo cp -R $CHROOT_DIR test_chroot || exit 1
fi

# Reset the chroot (this is for quick tests only; more reliable method
# is to delete and start over)
sudo cp yarp-*.deb test_chroot/tmp || exit 1
run_in_chroot test_chroot "yes | apt-get remove yarp" 

DEPENDENCIES_DISTRIB="DEPENDENCIES_${PLATFORM_KEY}"
BACKPORTS_URL_DISTRIB="BACKPORTS_URL_${PLATFORM_KEY}"
if [ "${!BACKPORTS_URL_DISTRIB}" != "" ]; then
  echo "Using backports from ${!BACKPORTS_URL_DISTRIB}"
  run_in_chroot test_chroot "echo 'deb ${!BACKPORTS_URL_DISTRIB} ${PLATFORM_KEY}-backports main' > /etc/apt/sources.list.d/backports.list" || exit 1
  run_in_chroot test_chroot "apt-get update" || exit 1
fi

# Install tool to load .deb and its dependencies.  Not available on etch
run_in_chroot test_chroot "yes | apt-get install gdebi-core" || {
    echo "=============================================================="
    echo "= gdebi-core not available"
    echo "= you will need to test by hand for this platform, as follows:"
    echo "  sudo chroot $CHROOT_DIR"
    echo "  cd /tmp"
    echo "  # install *.deb"
    exit 1
}
#run_in_chroot test_chroot "cd tmp && gdebi --non-interactive *.deb" || exit 1

# _AC_

run_in_chroot test_chroot "apt-get install gdebi-core" || exit 1
run_in_chroot test_chroot "gdebi -n /tmp/$YARP_PACKAGE" || exit 1

echo "To enter test chroot, run: ($YARP_PACKAGE)"
echo "  sudo chroot $YARP_PACKAGE_DIR/test_chroot"
