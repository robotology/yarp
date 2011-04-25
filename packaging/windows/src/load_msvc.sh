#/bin/bash

##############################################################################
#
# Copyright: (C) 2011 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
# Figure out the environment needed for a particular version of Visual Studio
# 

# Get SETTINGS_* variables (paths) from cache
source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

echo "Compiler $3 variant $2"

# Run msbuild.bat, a batch script that can wrap a vsvars.bat call and
# pull out the PATH, INCLUDE, and LIB variables it sets.
$SETTINGS_SOURCE_DIR/src/msbuild.bat "$1" "$2" || {
	echo "Failed to run $1"
	exit 1
}
if [ ! -e msbuild_path.sh ] ; then
	echo "Failed to get PATH from $1"
	exit 1
fi

# check for redistributable material
dir=`dirname "$1"`
dir="$dir/redist"
if [ -e "$dir" ]; then
	REDIST_PATH="$dir"
fi

# Cache compiler paths and variables, for dependent packages to read
NEW_PATH=`bash ./msbuild_path.sh | grep "."`
NEW_INCLUDE=`bash ./msbuild_include.sh | grep "."`
NEW_LIB=`bash ./msbuild_lib.sh | grep "."`
(
	echo "export PATH='$NEW_PATH'"
	echo "export INCLUDE='$NEW_INCLUDE'"
	echo "export LIB='$NEW_LIB'"
	if [ ! "k$REDIST_PATH" = "k" ]; then
		echo "export REDIST_PATH='$REDIST_PATH'"
	fi
	echo "export COMPILER_FAMILY=msvc"
) > compiler_config_${3}_${2}.sh

# Clean up
rm msbuild_*.sh || {
	rm compiler_config_${3}_${2}.sh
	exit 1
}
