#!/bin/bash

##############################################################################
#
# Copyright: (C) 2011 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
# Common utility to set compiler options.  Not intended to be called
# directly by the user, but rather to be sourced by build_*.sh scripts.
#
# Command line arguments: 
#   process_options.sh ${OPT_COMPILER} ${OPT_VARIANT} ${OPT_BUILD}
# Example:
#   process_options.sh v10 x86 Release
#
# Input variables:
#   settings.sh and $SETTINGS_BUNDLE_FILENAME are assumed to have been
#   sourced.
#
# Input files:
#   compiler_config_${OPT_COMPILER}_${OPT_VARIANT}.sh
#     cached variables needed by the given compiler version, generated
#     by packages/windows/manage.sh
#
# Output variables:
#   OPT_GCCLIKE     True for gcc or mingw compilers
#   OPT_PLATFORM    MSVC name for platform, if appropriate (v100, v90, v80).
#   OPT_VCNNN       Another variant of MSVC name for platform (VC100, VC90,...)
#   LIBEXT          Extension used for libraries (."lib", ."dll.a")
#   OPT_BUILDER     Program to use for building (msbuild.exe, make)
#   OPT_PLATFORM_COMMAND       Option for selecting platform
#   OPT_CONFIGURATION_COMMAND  Option for selecting build type during build
#   OPT_CMAKE_OPTION           Option for selecting build type during cmake
#   OPT_VC_REDIST_CRT          Patch to MSVC redistributables, if available
#   OPT_GENERATOR   CMake generator to use
#
# Output functions:
#   target_name     Convert argument to target name (e.g. add .sln for MSVC)
#   target_lib_name Add appropriate library prefix and extension


OPT_COMPILER=$1
OPT_VARIANT=$2
OPT_BUILD=$3

OPT_GCCLIKE=false

function target_name {
	if $OPT_GCCLIKE ; then
		TARGET=""
	else
		TARGET="$1.sln"
	fi
}

function target_lib_name {
	if $OPT_GCCLIKE ; then
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

	source compiler_config_${OPT_COMPILER}_${OPT_VARIANT}.sh || {
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
	if [ "$OPT_COMPILER" == "v12" ] ; then
		OPT_PLATFORM=v120
		OPT_VCNNN="VC120"
		OPT_HUMAN_PLATFORM_NAME="msvc12"
	fi
	if [ "$OPT_COMPILER" = "v11" ] ; then
		OPT_PLATFORM=v110
		OPT_VCNNN="VC110"
		OPT_HUMAN_PLATFORM_NAME="msvc11"
	fi
	
	if [ "k$OPT_COMPILER" = "kv10" ] ; then
		OPT_PLATFORM=v100
		OPT_VCNNN="VC100"
		OPT_HUMAN_PLATFORM_NAME="msvc10"
	fi
	if [ "k$OPT_COMPILER" = "kv9" ] ; then
		OPT_PLATFORM=v90
		OPT_VCNNN="VC90"
		OPT_HUMAN_PLATFORM_NAME="msvc9"
	fi
	if [ "k$OPT_COMPILER" = "kv8" ] ; then
		OPT_PLATFORM=v80
		OPT_VCNNN="VC80"
		OPT_HUMAN_PLATFORM_NAME="msvc8"
	fi
	if [ "k$OPT_COMPILER" = "kmingw" ] ; then
		OPT_PLATFORM=mingw
		OPT_HUMAN_PLATFORM_NAME="mingw"
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
	if [ "$OPT_COMPILER" == "v12" ] ; then
		if [ "$OPT_VARIANT" == "x86" ] ; then
			OPT_GENERATOR="Visual Studio 12"
		elif [ "$OPT_VARIANT" == "x64" ] || [ "$OPT_VARIANT" == "amd64" ] || [ "$OPT_VARIANT" == "x86_amd64" ] ; then
			OPT_GENERATOR="Visual Studio 12 Win64"
		fi		
	fi
	if [ "$OPT_COMPILER" == "v11" ] ; then
		if [ "$OPT_VARIANT" == "x86" ] ; then
			OPT_GENERATOR="Visual Studio 11"
		elif [ "$OPT_VARIANT" == "x64" ] || [ "$OPT_VARIANT" == "amd64" ] || [ "$OPT_VARIANT" == "x86_amd64" ] ; then
			OPT_GENERATOR="Visual Studio 11 Win64"
		fi		
	fi
	if [ "$OPT_COMPILER" == "v10" ] ; then
		if [ "$OPT_VARIANT" == "x86" ] ; then
			OPT_GENERATOR="Visual Studio 10"
		elif [ "$OPT_VARIANT" == "x64" ] || [ "$OPT_VARIANT" == "amd64" ] || [ "$OPT_VARIANT" == "x86_amd64" ] ; then
			OPT_GENERATOR="Visual Studio 10 Win64"
		fi
	fi
	if [ "$OPT_COMPILER" == "v9" ] ; then
		if [ "$OPT_VARIANT" == "x86" ] ; then
			OPT_GENERATOR="Visual Studio 9 2008"
		elif [ "$OPT_VARIANT" == "x64" ] || [ "$OPT_VARIANT" == "amd64" ] || [ "$OPT_VARIANT" == "x86_amd64" ] ; then
			OPT_GENERATOR="Visual Studio 9 2008 Win64"
		fi
	fi
	if [ "$OPT_COMPILER" == "v8" ] ; then
		if [ "$OPT_VARIANT" == "x86" ] || [ "$OPT_VARIANT" == "x64" ] || [ "$OPT_VARIANT" == "amd64" ] || [ "$OPT_VARIANT" == "x86_amd64" ] ; then
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

