#!/bin/bash

##############################################################################
#
# Copyright: (C) 2011 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
# Create a basic chroot for a particular distribution.  This chroot
# is copied for compiling or testing YARP.  It is not used directly
# for compiling or testing, but instead kept in a pristine state, in
# order to reduce the time needed when wiping a compilation/test and 
# starting over.
#
# Command line arguments: 
#   build_chroot.sh ${OPT_PLATFORM} ${OPT_CHROOT_DIRECTORY}
# Example:
#   build_chroot.sh etch chroot_etch
#
# Inputs:
#   settings.sh (general configuration)
#
# Outputs:
#   ${OPT_CHROOT_DIRECTORY}
#      a chroot with the basics for compiling or testing yarp
#   chroot_${OPT_PLATFORM}.sh
#      records name and directory of chroot

BUILD_DIR=$PWD

source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

platform=$1
dir=$2

if [ "k$dir" = "k" ]; then
    echo "Call as: build_chroot.sh <platform> <dir>"
    exit 1
fi

# Load the configuration of the desired platform
source ./config_$platform.sh || {
	echo "No platform configuration file found"
	exit 1
}

# If Ubuntu-flavored, make sure to include Universe (to get ACE)
if [ "k$PLATFORM_IS_UBUNTU" = "ktrue" ]; then
    PLATFORM_COMPONENTS="--components=main,universe"
fi

# Create chroot if it does not already exist
if [ ! -e $dir ]; then
    sudo debootstrap --variant=buildd $PLATFORM_COMPONENTS $platform $dir $PLATFORM_MIRROR || {
	sudo rm -rf chroot_$platform.fail; 
	sudo mv chroot_$platform chroot_$platform.fail 
	exit 1
    }
fi

# Record chroot configuration for future reference
(
	echo "export CHROOT_NAME='$platform'"
	echo "export CHROOT_DIR='$PWD/$dir'"
) > $BUILD_DIR/chroot_${platform}.sh
