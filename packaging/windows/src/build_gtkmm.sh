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

GTKMM_VERSION_TAG="BUNDLE_GTKMM_VERSION_${OPT_COMPILER}_${OPT_VARIANT}"
if [ "${!GTKMM_VERSION_TAG}" == "" ]; then
	echo "Warning no GTKMM version defined for ${GTKMM_VERSION_TAG}"
	exit 0
fi

GTKMM_PATH="gtkmm-${!GTKMM_VERSION_TAG}_${OPT_VARIANT}"
GTKMM_FILE="${GTKMM_PATH}.zip" 
GTKMM_URL="${BUNDLE_EXT_REPO_URL}/${GTKMM_FILE}" 

if [ ! -e "$GTKMM_FILE" ]; then
	wget $GTKMM_URL || {
		echo "Cannot fetch GTKMM file $GTKMM_URL"
		exit 1
	}
fi

if [ ! -d "$GTKMM_PATH" ] ; then
    unzip $GTKMM_FILE || {
	  	echo "Cannot unpack GTKMM file $GTKMM_FILEMM"
		exit 1
	}
fi

find $GTKMM_PATH -name "*.exe" -exec chmod u+x '{}' \;
find $GTKMM_PATH -name "*.dll" -exec chmod u+x '{}' \;

(
	GTKMM_DIR=$(cygpath --mixed "${BUILD_DIR}/${GTKMM_PATH}")
	echo "export GTKMM_DIR='$GTKMM_DIR'"
	echo "export GTKMM_PATH='$GTKMM_PATH'"
	echo "export GTK_BASEPATH='$GTKMM_DIR'"
	echo "export GTKMM_BASEPATH='$GTKMM_DIR'"
) > $BUILD_DIR/gtkmm_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh