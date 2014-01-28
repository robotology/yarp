#!/bin/bash

##############################################################################
#
# Copyright: (C) 2011 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
# Download a precompiled version of nsis.
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

if [ "k$BUNDLE_NSIS_VERSION" = "k" ]; then
	echo "Set BUNDLE_NSIS_VERSION"
	exit 1
fi

# Go ahead and download
fname=nsis-$BUNDLE_NSIS_VERSION
fname_patch=nsis-patch-$BUNDLE_NSIS_VERSION
if [ ! -e $fname.zip ]; then
	if [ "$BUNDLE_NSIS_ZIP" != "" ]
	then
		zipname="$BUNDLE_NSIS_ZIP"
	else
		zipname="http://downloads.sourceforge.net/project/nsis/NSIS%202/$BUNDLE_NSIS_VERSION/nsis-$BUNDLE_NSIS_VERSION.zip"
	fi
	wget -O $fname.zip $zipname || (
		echo "Cannot fetch NSIS"
		rm -f $fname.zip
		exit 1
	)
fi

# Also fetch a patched NSIS that can deal with longer strings
if [ ! -e $fname_patch.zip ]; then
	if [ "$BUNDLE_NSIS_PATCHES_ZIP" != "" ]
	then
		zipname="$BUNDLE_NSIS_PATCHES_ZIP"
	else
		zipname="http://downloads.sourceforge.net/project/nsis/NSIS%202/$BUNDLE_NSIS_VERSION/nsis-$BUNDLE_NSIS_VERSION-strlen_8192.zip"
	fi
	wget -O $fname_patch.zip $zipname || (
		echo "Cannot fetch NSIS patch"
		rm -f $fname_patch.zip
		exit 1
	)
fi

# Unpack if needed, folding in patched executable
if [ ! -d $fname ]; then
	unzip $fname.zip || {
		echo "Cannot unpack NSIS"
		cd $BUILD_DIR
		rm -rf $fname
		exit 1
	}
	cd $fname || exit 1
	unzip -o ../$fname_patch.zip || {
		echo "Cannot patch NSIS"
		cd $BUILD_DIR
		rm -rf $fname
		exit 1
	}
    chmod u+x makensis.exe || {
		echo "Cannot find NSIS executable"
		cd $BUILD_DIR
		rm -rf $fname
		exit 1
	}
fi

# Cache NSIS-related paths and variables, for dependent packages to read
cd $BUILD_DIR
(
	NSIS_DIR=`cygpath --mixed $PWD/$fname`
	echo "export NSIS_DIR='$NSIS_DIR'"
	echo "export NSIS_BIN='$NSIS_DIR/makensis.exe'"
) > $BUILD_DIR/nsis_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
