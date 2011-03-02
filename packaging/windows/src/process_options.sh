#!/bin/bash

compiler=$1
variant=$2
build=$3

if [ "k$build" = "k" ]; then
	echo "Provide options: compiler variant build"
	echo "For example: v10 x86 Release"
	exit 1
fi

source compiler_config_${compiler}_${variant}.sh || {
	echo "Compiler settings not found"
	exit 1
}

if [ "k$compiler" = "kv10" ] ; then
	platform=v100
else 
	echo "Please set platform for compiler $compiler in process_options.sh"
	exit 1
fi

PLATFORM_COMMAND="/p:PlatformToolset=$platform"
CONFIGURATION_COMMAND="/p:Configuration=$build"

generator=""
if [ "k$compiler" = "kv10" ] ; then
	if [ "k$variant" = "kx86" ] ; then
		generator="Visual Studio 10"
	fi
fi
if [ "k$generator" = "k" ] ; then 
	echo "Please set generator for compiler $compiler variant $variant in process_options.sh"
	exit 1
fi

BUILDER="msbuild.exe"
