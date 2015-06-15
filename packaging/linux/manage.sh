#!/bin/bash

##############################################################################
#
# Copyright: (C) 2011 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
# This script configures a build directory.  To use it, do something like:
#   mkdir build
#   cd build
#   $SOURCE_DIR/manage.sh
# where $SOURCE_DIR is the path to this script.
#
# This will list a set of "bundles" that can be built.  Each named
# bundle $BUNDLE corresponds to specific versions of software packages 
# used to put together a named binary release.
#
# Each bundle $BUNDLE corresponds to a file named:
#   $SOURCE_DIR/conf/bundle-$BUNDLE.sh
# To see the versions of software involved in a bundle, read that file.
# If no bundle file exists that does what you want, make a new one.
# Then, in your build directory $BUILD_DIR, do:
#   $SOURCE_DIR/manage.sh $BUNDLE
# This will produce at least the following files:
#   $BUILD_DIR/Makefile     # targets for building release
#   $BUILD_DIR/settings.sh  # cache of build variables, including $BUNDLE
#

# For simplicity, make sure this script is not called in-place
if [ -e manage.sh ]; then
	echo "Please call from a build directory"
	exit 1
fi

# Check bash isn't too old to support associative arrays
declare -A check_for_associative_arrays || (
	echo "Update cygwin bash version"
	exit 1
)

# Set up build and source directory
BUILD_DIR=$PWD
relative_dir="`dirname $0`"
cd "$relative_dir"
SOURCE_DIR=$PWD
cd $BUILD_DIR

# If no argument given, report available "bundles" (lists of software 
# package versions) and exit.  We generate some temporary files to
# enable tab completion.
BUNDLES="`cd $SOURCE_DIR/conf; ls -1 bundle*.sh | sed "s/\.sh//"`"
if [ "k$1" = "k" ]; then
	echo "Please specify a bundle name.  One of these:"
	# Support tab completion
	for b in $BUNDLES; do
	    echo -n " $b"
	    echo "$0 $b" > $b
	    chmod u+x $b
	done
	echo " "
	exit 1
else
	# Remove tab completion
	for b in $BUNDLES; do
	    rm -f $b
	done
fi

# Load bundle settings
BUNDLE_NAME="$1"
BUNDLE_FILENAME="$SOURCE_DIR/conf/$1.sh"
if [ ! -e "$BUNDLE_FILENAME" ]; then
	echo "Cannot find $BUNDLE_FILENAME"
	exit 1
fi
source $BUNDLE_FILENAME

# Start preparing Makefile.  The default target is to rerun
# this script with the desired bundle.
MAKEFILE=$BUILD_DIR/Makefile
(
	echo "default:"
	echo -e "\t$0 $1\n"
) > $MAKEFILE
creation_list="Makefile"

# Load default list of platforms
source $SOURCE_DIR/conf/compilers.sh || exit 1

# Override list of platforms with local configuration, if available
if [ -e $SOURCE_DIR/conf/compilers_local.sh ]; then
	source $SOURCE_DIR/conf/compilers_local.sh || exit 1
fi

# Cache important variables in $BUILD_DIR/settings.sh
cd $BUILD_DIR
(
	echo "export SOURCE_DIR='$SOURCE_DIR'"
	echo "export BUNDLE_NAME='$BUNDLE_NAME'"
	echo "export BUNDLE_FILENAME='$BUNDLE_FILENAME'"
	echo "export TESTING=$TESTING"
	echo "export YARP_REVISION=$YARP_REVISION"
) > $BUILD_DIR/settings.sh
creation_list="$creation_list, settings.sh"

# Check that debootstrap is available - key to build process
if sudo which debootstrap; then
    echo "Good, debootstrap is available"
else
    echo "Please install debootstrap, needed for building"
    sudo apt-get install debootstrap
fi

# Scan platforms for which scripts are available
PLATFORM_SCRIPTS=/usr/share/debootstrap/scripts
echo " "
echo "debootstrap scripts are available on this machine for:"
ls $PLATFORM_SCRIPTS
echo " "

# Add build targets to Makefile
all_targets=""
VARIANTS=""
for platform in $PLATFORMS; do
    # Pick up correct script for platform
	    PLATFORM_FILE="$PLATFORM_SCRIPTS/$platform"
    if [ ! -e "$PLATFORM_FILE" ]; then
	echo "Do not know how to make $platform"
	exit 1
    fi
    echo "Adding targets for: $platform"
    for hardware in $HARDWARE; do
	variant="${platform}_${hardware}"
	skip_t=SKIP_${variant}
	skip=${!skip_t}
	if [ -n "$skip" ]; then
	    continue
	fi
	VARIANTS="$VARIANTS $variant"
        # Figure out mirror in use: Debian or Ubuntu?
        # Will need to know because Ubuntu keeps ACE in Universe rather 
	# than main repository
	{
	    PLATFORM_MIRROR_t=${platform}_MIRROR
	    PLATFORM_MIRROR=${!PLATFORM_MIRROR_t}
	    echo "PLATFORM_KEY=$platform"
	    echo "PLATFORM_HARDWARE=$hardware"
	    echo "PLATFORM_MIRROR=$PLATFORM_MIRROR"
	    grep -q "ubuntu\.com" $PLATFORM_FILE && echo "PLATFORM_IS_UBUNTU=true"
	    grep -q "ubuntu\.com" $PLATFORM_FILE || echo "PLATFORM_IS_DEBIAN=true"			
	} > config_$variant.sh
	creation_list="$creation_list, config_$variant.sh"
    
        # Update Makefile with useful targets
	all_targets="$all_targets yarp_$variant.txt test_$variant.txt"
	(
	    echo "chroot_$variant.txt:"
	    echo -e "\t$SOURCE_DIR/src/build_chroot.sh $variant chroot_$variant && touch chroot_$variant.txt\n"
	    echo "yarp_$variant.txt: chroot_$variant.txt"
	    echo -e "\t$SOURCE_DIR/src/build_yarp.sh $variant yarp_$variant && touch yarp_$variant.txt\n"
	    echo "test_$variant.txt: yarp_$variant.txt"
	    echo -e "\t$SOURCE_DIR/src/test_yarp.sh $variant && touch test_$variant.txt\n"
	    targets=""
	) >> $BUILD_DIR/Makefile
    done
done

# Catch-all targets
all_targets="$all_targets all"
(
    echo -n "all:"
    for variant in $VARIANTS; do
	echo -n " chroot_$variant.txt"
    done
    for variant in $VARIANTS; do
	echo -n " yarp_$variant.txt"
    done
    for variant in $VARIANTS; do
	echo -n " test_$variant.txt"
    done
    echo -e "\n"
) >> $BUILD_DIR/Makefile

echo " "
echo "Prepared $creation_list"
echo "Makefile targets include$all_targets"
