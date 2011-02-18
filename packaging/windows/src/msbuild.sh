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

NEW_PATH=`bash ./msbuild_path.sh | grep "."`
(
	echo "export PATH='$NEW_PATH'"
	echo '# echo PATH is "$PATH" - checking for msbuild:'
	echo "# which msbuild.exe"
) > compiler_config_${3}_${2}.sh

# clean up
rm msbuild_path.sh || {
	rm compiler_config_${3}_${2}.sh
	exit 1
}
