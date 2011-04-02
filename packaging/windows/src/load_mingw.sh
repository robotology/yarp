#!/bin/bash

source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

echo "Compiler $3 variant $2"

p=`cygpath -u $1`
(
	echo "export RESTRICTED_PATH='$p/bin:$p/msys/1.0/bin'"
	echo "export COMPILER_FAMILY=mingw"
) > compiler_config_${3}_${2}.sh
