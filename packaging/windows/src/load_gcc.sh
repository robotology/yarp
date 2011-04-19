#!/bin/bash

source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

echo "Compiler $3 variant $2"

(
	echo "# Nothing special needed for gcc"
	echo "export COMPILER_FAMILY=gcc"
) > compiler_config_${3}_${2}.sh
