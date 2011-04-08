#!/bin/bash

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

source $SOURCE_DIR/conf/site.sh || {
	echo "Cannot find conf/site.sh"
	exit 1
}

if [ -e "$SOURCE_DIR/conf/site_local.sh" ]; then
	source $SOURCE_DIR/conf/site_local.sh
fi

if [ "k$site_dir" = "k" ]; then
	echo "Please read $SOURCE_DIR/conf/site.sh"
	exit 1
fi

source yarp_core_package_${compiler}_${variant}_${build}.sh || {
	echo "Cannot find corresponding YARP package"
	exit 1
}

fname=transfer-$YARP_VERSION

fname2=$fname-$compiler-$variant-$build

mkdir -p $fname2
cd $fname2 || exit 1
OUT_DIR=$PWD

for f in `cd $YARP_CORE_PACKAGE_DIR; ls *.exe`; do
	YARP_CORE_PACKAGE_NAME="$f"
	target="$site_user@$site:$site_dir"
	echo "Getting package"
	echo "  Directory: $YARP_CORE_PACKAGE_DIR"
	echo "  File: $YARP_CORE_PACKAGE_NAME"
	echo "Sending package"
	echo "  Target: $target"
	scp $YARP_CORE_PACKAGE_DIR/$YARP_CORE_PACKAGE_NAME $target || exit 1
done

(
	echo "export TRANSFER_PACKAGE_DIR='$YARP_CORE_PACKAGE_DIR'"
	echo "export TRANSFER_PACKAGE_NAME='$YARP_CORE_PACKAGE_NAME'"
) > $BUILD_DIR/transfer_${compiler}_${variant}_${build}.sh

