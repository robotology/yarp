#!/bin/bash

# Actually, we just download a precompiled gtkmm.
# We could compile it, but it doesn't seem like we need to do better distributing it
# than what already exists.

BUILD_DIR=$PWD

source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

source $SETTINGS_BUNDLE_FILENAME || {
	echo "Bundle settings not found"
	exit 1
}

source $SETTINGS_SOURCE_DIR/src/process_options.sh $* || {
	echo "Cannot process options"
	exit 1
}

if [ "k$BUNDLE_GTKMM_VERSION" = "k" ]; then
	#BUNDLE_GTKMM_VERSION=2.22.0-2
	echo "Set GTKMM version"
	exit 1
fi

fname=gtkmm-$BUNDLE_GTKMM_VERSION

if [ ! -e $fname.exe ]; then
	GTKMM_DIR=`echo $BUNDLE_GTKMM_VERSION | sed "s/\.[-0-9]*$//"`
	wget -O $fname.exe http://ftp.gnome.org/pub/GNOME/binaries/win32/gtkmm/$GTKMM_DIR/gtkmm-win32-devel-$BUNDLE_GTKMM_VERSION.exe || (
		echo "Cannot fetch GTKMM"
		rm -f $fname.exe
		exit 1
	)
fi

if [ ! -d $fname ]; then
	mkdir -p $fname
	cd $fname
	7z x ../$fname.exe || {
		echo "Cannot unpack GTKMM"
		cd $BUILD_DIR
		rm -rf $fname
		exit 1
	}
fi

cd $BUILD_DIR
if [ ! -e $fname/include ]; then
	# important rename
	cd $fname
	mv *_OUTDIR include || {
		echo "Failed to rename include directory"
		#cd $BUILD_DIR
		#rm -rf $fname
		exit 1
	} 
fi

cd $BUILD_DIR

(
	GTKMM_DIR=`cygpath --mixed "$PWD/$fname"`
	echo "export GTKMM_DIR='$GTKMM_DIR'"
	echo "export GTK_BASEPATH='$GTKMM_DIR'"
	echo "export GTKMM_BASEPATH='$GTKMM_DIR'"
) > $BUILD_DIR/gtkmm_${compiler}_${variant}_${build}.sh
