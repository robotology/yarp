#/bin/bash

source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

echo "Compiler $3 variant $2"

$SOURCE_DIR/src/msbuild.bat "$1" "$2" || {
	echo "Failed to run $1"
	exit 1
}

if [ ! -e msbuild_path.sh ] ; then
	exit 1
fi

dir=`dirname $1`
dir="$dir/redist"
if [ -e "$dir" ]; then
	REDIST_PATH="$dir"
fi

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

# clean up
rm msbuild_path.sh || {
	rm compiler_config_${3}_${2}.sh
	exit 1
}
