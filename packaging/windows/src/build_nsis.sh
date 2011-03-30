#!/bin/bash

# Actually, we just download a precompiled nsis.

BUILD_DIR=$PWD

source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

source $BUNDLE_FILENAME || {
	echo "Bundle settings not found"
	exit 1
}

source $SOURCE_DIR/src/process_options.sh $* || {
	echo "Cannot process options"
	exit 1
}

if [ "k$NSIS_VERSION" = "k" ]; then
	#NSIS_VERSION=2.46
	echo "Set NSIS_VERSION"
	exit 1
fi

fname=nsis-$NSIS_VERSION
fname_patch=nsis-patch-$NSIS_VERSION

if [ ! -e $fname.zip ]; then
	zipname="http://downloads.sourceforge.net/project/nsis/NSIS%202/$NSIS_VERSION/nsis-$NSIS_VERSION.zip"

	wget -O $fname.zip $zipname || (
		echo "Cannot fetch NSIS"
		rm -f $fname.zip
		exit 1
	)
fi

if [ ! -e $fname_patch.zip ]; then
	zipname="http://downloads.sourceforge.net/project/nsis/NSIS%202/$NSIS_VERSION/nsis-$NSIS_VERSION-strlen_8192.zip"

	wget -O $fname_patch.zip $zipname || (
		echo "Cannot fetch NSIS patch"
		rm -f $fname_patch.zip
		exit 1
	)
fi

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

cd $BUILD_DIR

(
	NSIS_DIR=`cygpath --mixed $PWD/$fname`
	echo "export NSIS_DIR='$NSIS_DIR'"
	echo "export NSIS_BIN='$NSIS_DIR/makensis.exe'"
) > $BUILD_DIR/nsis_${compiler}_${variant}_${build}.sh
