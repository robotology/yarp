#!/bin/bash

# Actually, we just download a qt binaries package.

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

QT_VERSION_TAG="BUNDLE_QT_VERSION_${OPT_COMPILER}_${OPT_VARIANT}"
if [ "${!QT_VERSION_TAG}" == "" ]; then
	echo "Warning no QT version defined for ${QT_VERSION_TAG}"
	exit 0
fi

QT_PATH="Qt-${!QT_VERSION_TAG}_${OPT_COMPILER}_${OPT_VARIANT}"
QT_FILE="${QT_PATH}.zip"
QT_URL="${BUNDLE_EXT_REPO_URL}/${QT_FILE}" 

if [ ! -e "$QT_FILE" ]; then
	wget $QT_URL || {
		echo "Cannot fetch QT file $QT_URL"
		exit 1
	}
fi

if [ ! -d "$QT_PATH" ] ; then
    unzip $QT_FILE || {
	  	echo "Cannot unpack QT file $QT_FILE"
		exit 1
	}
fi

find $QT_PATH -name "*.exe" -exec chmod u+x '{}' \;
find $QT_PATH -name "*.dll" -exec chmod u+x '{}' \;

(
	QT_DIR=$(cygpath --mixed "${BUILD_DIR}/${QT_PATH}")
	Qt5_DIR=$(cygpath --mixed "${BUILD_DIR}/${QT_PATH}/lib/cmake/Qt5")
	
	echo "export QT_DIR='$QT_DIR'"
	echo "export QT_PATH='$QT_PATH'"
	echo "export Qt5_DIR='$Qt5_DIR'"
) > $BUILD_DIR/qt_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
