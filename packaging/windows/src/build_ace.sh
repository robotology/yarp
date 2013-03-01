#!/bin/bash

##############################################################################
#
# Copyright: (C) 2011 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
# Build ACE from source.
# 

BUILD_DIR=$PWD

# Get SETTINGS_* variables (paths) from cache
source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

# Get BUNDLE_* variables (software versions) from the bundle file
source $SETTINGS_BUNDLE_FILENAME || {
	echo "Bundle settings not found"
	exit 11
}

# GET OPT_* variables (build options) by processing our command-line options
source $SETTINGS_SOURCE_DIR/src/process_options.sh $* || {
	echo "Cannot process options"
	exit 1
}

# Building ACE is quirky.  We use the "traditional" method rather than
# autotools, since that method seems better maintained.  Here we figure
# out the correct ACE project file (or makefile) to use, given the compiler 
# options
# Visual Studio 11 not directly supported so we use ACE_vc10. We set
###
if [ "$OPT_COMPILER" == "v11" ] ; then
    echo "WARNING: ACE does not yet provide visual studio 11 project files so you have to manually open and convert ACE_vc10.vcxproj"
	convert=1
	pname=ACE_vc10.sln #ACE_vc10.vcxprojfi
	if [ "$OPT_VARIANT" == "x64" ] || [ "$OPT_VARIANT" == "amd64" ] || [ "$OPT_VARIANT" == "x86_amd64" ] ; then
		OPT_PLATFORM_COMMAND="/p:Platform=x64"
	elif [ "$OPT_VARIANT" == "x86" ] ; then
		OPT_PLATFORM_COMMAND="/p:Platform=Win32"
	fi
fi
if [ "$OPT_COMPILER" == "v10" ] ; then
	pname=ACE_vc10.vcxproj
	if [ "$OPT_VARIANT" == "x64" ] || [ "$OPT_VARIANT" == "amd64" ] || [ "$OPT_VARIANT" == "x86_amd64" ] ; then
		OPT_PLATFORM_COMMAND="/p:Platform=x64"
	elif [ "$OPT_VARIANT" == "x86" ] ; then
		OPT_PLATFORM_COMMAND="/p:Platform=Win32"
	fi
fi
if [ "$OPT_COMPILER" == "v9" ] ; then
	pname=ACE_vc9.vcproj
	OPT_BUILDER=$OPT_BUILDER_VCBUILD
	OPT_PLATFORM_COMMAND=$OPT_PLATFORM_COMMAND_VCBUILD
	if [ "$OPT_VARIANT" == "x64" ] || [ "$OPT_VARIANT" == "amd64" ] || [ "$OPT_VARIANT" == "x86_amd64" ] ; then
		OPT_PLATFORM_COMMAND="/platform=x64"
	elif [ "$OPT_VARIANT" == "x86" ] ; then
		OPT_PLATFORM_COMMAND="/platform=Win32"
	fi
	OPT_CONFIGURATION_COMMAND="${OPT_CONFIGURATION_COMMAND_VCBUILD}"
fi
###
if [ "k$OPT_COMPILER" = "kv8" ] ; then
	pname=ACE_vc8.vcproj
	OPT_BUILDER=$OPT_BUILDER_VCBUILD
	OPT_PLATFORM_COMMAND=$OPT_PLATFORM_COMMAND_VCBUILD
	OPT_CONFIGURATION_COMMAND="${OPT_CONFIGURATION_COMMAND_VCBUILD}"
fi
LIBPRE=""
if $OPT_GCCLIKE; then
	LIBPRE="lib"
	pname=" "
	OPT_BUILDER="make ACE"
	OPT_PLATFORM_COMMAND=
	OPT_CONFIGURATION_COMMAND=
	if [ "k$OPT_BUILD" = "kDebug" ]; then
		OPT_CONFIGURATION_COMMAND="debug=1"
	fi
fi
if [ "k$pname" = "k" ] ; then 
	echo "Please set project name for compiler $OPT_COMPILER in build_ace.sh"
	exit 1
fi

# Download and store an ACE tarball
fname=ACE-$BUNDLE_ACE_VERSION
if [ ! -e $fname.tar.gz ]; then
	wget http://download.dre.vanderbilt.edu/previous_versions/$fname.tar.gz || (
		echo "Cannot fetch ACE"
		exit 1
	)
fi

# Unpack the source.  I don't know how to do out-of-source builds with
# ACE's traditional build method on Windows, so instead unpack the source
# as many times as needed.
fname2=$fname-$OPT_COMPILER-$OPT_VARIANT
if $OPT_GCCLIKE; then 
	name2=$fname-$OPT_COMPILER-$OPT_VARIANT-$OPT_BUILD
fi
if [ ! -e $fname2 ]; then
	echo "PATH is $PATH"
	tar xzvf $fname.tar.gz || (
		echo "Cannot unpack ACE"
		exit 1
	)
	mv ACE_wrappers $fname2 || exit 1
fi

# Configure the source, adding an appropriate config.h
cd $fname2 || exit 1
export ACE_ROOT=$PWD
if [ ! -e $ACE_ROOT/ace/config.h ] ; then
	echo "Creating $ACE_ROOT/ace/config.h"	
	cd $ACE_ROOT/ace
	echo '#include "ace/config-win32.h"' > config.h
fi
if [ "k$COMPILER_FAMILY" = "kmingw" ]; then
	cd $ACE_ROOT
	if [ ! -e include/makeinclude/platform_macros.GNU ]; then
		echo "Creating $ACE_ROOT/include/makeinclude/platform_macros.GNU"	
		cd $ACE_ROOT/include/makeinclude
		echo 'include $(ACE_ROOT)/include/makeinclude/platform_mingw32.GNU' > platform_macros.GNU
	fi
fi

# Make sure that the desired project file is present
cd $ACE_ROOT/ace
echo $PWD
if [ ! -e $pname ]; then
	echo "Could not find $pname"
	exit 1
fi

# Carefully run the build.  MINGW is fiddly to run from CYGWIN, so we 
# may need to zap the PATH

echo "$OPT_BUILDER $pname $OPT_CONFIGURATION_COMMAND $OPT_PLATFORM_COMMAND"
ACE_DIR=`cygpath --mixed "$ACE_ROOT"`
{
if [ ! "k$RESTRICTED_PATH" = "k" ]; then
	ACE_ROOT=`cygpath -m $ACE_ROOT`
	PATH="$RESTRICTED_PATH"
	echo "ACE_ROOT set to $ACE_ROOT"
	echo "PATH set to $PATH"
fi

if [ "k$convert" = "k1" ]; then
    echo "Upgrading solution file"
	devenv $pname /upgrade || exit 1
fi

$OPT_BUILDER $pname $OPT_CONFIGURATION_COMMAND $OPT_PLATFORM_COMMAND || exit 1
}

# Figure out the library name, and normalize its location.
libname=ACE
if [ "k$OPT_BUILD" = "kDebug" ]; then
	libname=ACEd
fi
cd $ACE_ROOT
if [ "k$COMPILER_FAMILY" = "kmingw" ]; then
	for f in `cd ace; ls *.dll.a`; do
		cp ace/$f lib/$f
	done
fi

# Cache ACE-related paths and variables, for dependent packages to read
(
	echo "export ACE_DIR='$ACE_DIR'"
	echo "export ACE_ROOT='$ACE_DIR'"
	echo "export ACE_LIBNAME='$LIBPRE$libname'"
) > $BUILD_DIR/ace_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
