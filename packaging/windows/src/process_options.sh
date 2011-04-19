#!/bin/bash

OPT_COMPILER=$1
OPT_VARIANT=$2
OPT_BUILD=$3

OPT_GCCLIKE=false

function target_name {
	if $gcc_like ; then
		TARGET=""
	else
		TARGET="$1.sln"
	fi
}

function target_lib_name {
	if $gcc_like ; then
		TARGET_LIB="lib$2.a"
	else
		TARGET_LIB="$1/$2.lib"
	fi
}

if [ ! "k$OPT_COMPILER" = "kany" ]; then

	if [ "k$OPT_BUILD" = "k" ]; then
		echo "Provide options: compiler variant build"
		echo "For example: v10 x86 Release"
		exit 1
	fi

	source compiler_config_${compiler}_${variant}.sh || {
		echo "Compiler settings not found"
		exit 1
	}

	if [ "k$COMPILER_FAMILY" = "kmingw" ] ; then
	    OPT_GCCLIKE=true
	elif [ "k$COMPILER_FAMILY" = "kgcc" ] ; then
	    OPT_GCCLIKE=true
	else
	    OPT_GCCLIKE=false
	fi

	if [ "k$COMPILER_FAMILY" = "kmingw" ] ; then
		LIBEXT="dll.a"
	elif [ "k$COMPILER_FAMILY" = "kgcc" ] ; then
		LIBEXT="so"
	else
		LIBEXT="lib"
	fi

	OPT_PLATFORM=""
	OPT_VCNNN=""
	if [ "k$OPT_COMPILER" = "kv10" ] ; then
		OPT_PLATFORM=v100
		OPT_VCNNN="VC100"
	fi
	if [ "k$OPT_COMPILER" = "kv9" ] ; then
		OPT_PLATFORM=v90
		OPT_VCNNN="VC90"
	fi
	if [ "k$OPT_COMPILER" = "kv8" ] ; then
		OPT_PLATFORM=v80
		OPT_VCNNN="VC80"
	fi
	if [ "k$OPT_COMPILER" = "kmingw" ] ; then
		OPT_PLATFORM=mingw
	fi
	if [ "k$OPT_COMPILER" = "kgcc" ] ; then
		OPT_PLATFORM=gcc
	fi
	if [ "k$OPT_PLATFORM" = "k" ]; then
		echo "Please set platform for compiler $OPT_COMPILER in process_options.sh"
		exit 1
	fi

	OPT_PLATFORM_COMMAND="/p:PlatformToolset=$OPT_PLATFORM"
	OPT_CONFIGURATION_COMMAND="/p:Configuration=$OPT_BUILD"

	OPT_GENERATOR=""
	if [ "k$OPT_COMPILER" = "kv10" ] ; then
		if [ "k$OPT_VARIANT" = "kx86" ] ; then
			OPT_GENERATOR="Visual Studio 10"
		fi
	fi
	if [ "k$OPT_COMPILER" = "kv9" ] ; then
		if [ "k$OPT_VARIANT" = "kx86" ] ; then
			OPT_GENERATOR="Visual Studio 9 2008"
		fi
	fi
	if [ "k$OPT_COMPILER" = "kv8" ] ; then
		if [ "k$OPT_VARIANT" = "kx86" ] ; then
			OPT_GENERATOR="Visual Studio 8 2005"
		fi
	fi
	if [ "k$OPT_COMPILER" = "kmingw" ] ; then
		OPT_GENERATOR="MSYS Makefiles"
	fi
	if [ "k$OPT_COMPILER" = "kgcc" ] ; then
		OPT_GENERATOR="Unix Makefiles"
	fi
	if [ "k$OPT_GENERATOR" = "k" ] ; then 
		echo "Please set generator for compiler $OPT_COMPILER variant $OPT_VARIANT in process_options.sh"
		exit 1
	fi

	OPT_VC_REDIST_CRT=""
	if [ ! "k$REDIST_PATH" = "k" ] ; then
		OPT_VC_REDIST_CRT="$REDIST_PATH/$OPT_VARIANT/Microsoft.$OPT_VCNNN.CRT"
		if [ ! -e "$OPT_VC_REDIST_CRT" ] ; then
			OPT_VC_REDIST_CRT=""
		fi
	fi

	OPT_BUILDER="msbuild.exe"
	OPT_BUILDER_VCBUILD="vcbuild.exe /useenv"
	OPT_PLATFORM_COMMAND_VCBUILD=""
	# this will need updating for non-x86
	OPT_CONFIGURATION_COMMAND_VCBUILD="$OPT_BUILD"

	if $OPT_GCCLIKE ; then
		OPT_BUILDER="make"
		OPT_PLATFORM_COMMAND=""
		OPT_CONFIGURATION_COMMAND=""
		OPT_CMAKE_OPTION="-DCMAKE_BUILD_TYPE=$OPT_BUILD"
	fi
fi

