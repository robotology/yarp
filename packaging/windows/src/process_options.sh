#!/bin/bash

compiler=$1
variant=$2
build=$3

function target_name {
	if [ "k$COMPILER_FAMILY" = "kmingw" ] ; then
		TARGET=""
	else
		TARGET="$1.sln"
	fi
}

function target_lib_name {
	if [ "k$COMPILER_FAMILY" = "kmingw" ] ; then
		TARGET_LIB="lib$2.a"
	else
		TARGET_LIB="$1/$2.lib"
	fi
}

if [ ! "k$compiler" = "kany" ]; then

	if [ "k$build" = "k" ]; then
		echo "Provide options: compiler variant build"
		echo "For example: v10 x86 Release"
		exit 1
	fi

	source compiler_config_${compiler}_${variant}.sh || {
		echo "Compiler settings not found"
		exit 1
	}

	if [ "k$COMPILER_FAMILY" = "kmingw" ] ; then
		LIBEXT="dll.a"
	else
		LIBEXT="lib"
	fi

	platform=""
	VCNNN=""
	if [ "k$compiler" = "kv10" ] ; then
		platform=v100
		VCNNN="VC100"
	fi
	if [ "k$compiler" = "kv9" ] ; then
		platform=v90
		VCNNN="VC90"
	fi
	if [ "k$compiler" = "kv8" ] ; then
		platform=v80
		VCNNN="VC80"
	fi
	if [ "k$compiler" = "kmingw" ] ; then
		platform=mingw
	fi
	if [ "k$platform" = "k" ]; then
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
	if [ "k$compiler" = "kv9" ] ; then
		if [ "k$variant" = "kx86" ] ; then
			generator="Visual Studio 9 2008"
		fi
	fi
	if [ "k$compiler" = "kv8" ] ; then
		if [ "k$variant" = "kx86" ] ; then
			generator="Visual Studio 8 2005"
		fi
	fi
	if [ "k$compiler" = "kmingw" ] ; then
		generator="MSYS Makefiles"
	fi
	if [ "k$generator" = "k" ] ; then 
		echo "Please set generator for compiler $compiler variant $variant in process_options.sh"
		exit 1
	fi

	VC_REDIST_CRT=""
	if [ ! "k$REDIST_PATH" = "k" ] ; then
		VC_REDIST_CRT="$REDIST_PATH/$variant/Microsoft.$VCNNN.CRT"
		if [ ! -e "$VC_REDIST_CRT" ] ; then
			VC_REDIST_CRT=""
		fi
	fi

	BUILDER="msbuild.exe"
	BUILDER_VCBUILD="vcbuild.exe /useenv"
	PLATFORM_COMMAND_VCBUILD=""
	# this will need updating for non-x86
	CONFIGURATION_COMMAND_VCBUILD="$build"

	if [ "k$compiler" = "kmingw" ] ; then
		BUILDER="make"
		PLATFORM_COMMAND=""
		CONFIGURATION_COMMAND=""
		CMAKE_OPTION="-DCMAKE_BUILD_TYPE=$build"
	fi
fi

