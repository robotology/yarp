#!/bin/bash

BUILD_DIR=$PWD

source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

source $BUNDLE_FILENAME || {
	echo "Bundle settings not found"
	exit 1
}

source $SOURCE_DIR/src/process_options.sh $* || {
	echo "Cannot process options"
	exit 1
}

if [ "k$compiler" = "kv10" ] ; then
	pname=ACE_vc10.vcxproj
fi
if [ "k$compiler" = "kv9" ] ; then
	pname=ACE_vc9.vcproj
	BUILDER=$BUILDER_VCBUILD
	PLATFORM_COMMAND=$PLATFORM_COMMAND_VCBUILD
	CONFIGURATION_COMMAND=$CONFIGURATION_COMMAND_VCBUILD
fi
if [ "k$compiler" = "kv8" ] ; then
	pname=ACE_vc8.vcproj
	BUILDER=$BUILDER_VCBUILD
	PLATFORM_COMMAND=$PLATFORM_COMMAND_VCBUILD
	CONFIGURATION_COMMAND=$CONFIGURATION_COMMAND_VCBUILD
fi
if [ "k$compiler" = "kmingw" ] ; then
	pname=" "
	BUILDER="make ACE"
	PLATFORM_COMMAND=
	CONFIGURATION_COMMAND=
	if [ "k$build" = "kDebug" ]; then
		CONFIGURATION_COMMAND="debug=1"
	fi
fi
if [ "k$pname" = "k" ] ; then 
	echo "Please set project name for compiler $compiler in build_ace.sh"
	exit 1
fi

fname=ACE-$ACE_VERSION

if [ ! -e $fname.tar.gz ]; then
	wget http://download.dre.vanderbilt.edu/previous_versions/$fname.tar.gz || (
		echo "Cannot fetch ACE"
		exit 1
	)
fi

# msbuild does not have out-of-source builds, so we duplicate source as needed
fname2=$fname-$compiler-$variant
# for mingw debug builds, seem to need different dirs
if [ "$COMPILER_FAMILY" = "mingw" ] ;then 
	name2=$fname-$compiler-$variant-$build
fi

if [ ! -e $fname2 ]; then
	echo "PATH is $PATH"
	tar xzvf $fname.tar.gz || (
		echo "Cannot unpack ACE"
		exit 1
	)
	mv ACE_wrappers $fname2 || exit 1
fi

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

cd $ACE_ROOT/ace
echo $PWD
if [ ! -e $pname ]; then
	echo "Could not find $pname"
	exit 1
fi

(
if [ ! "kRESTRICTED_PATH" = "k" ]; then
	ACE_ROOT=`cygpath -m $ACE_ROOT`
	PATH="$RESTRICTED_PATH"
	echo "ACE_ROOT set to $ACE_ROOT"
	echo "PATH set to $PATH"
fi
$BUILDER $pname $CONFIGURATION_COMMAND $PLATFORM_COMMAND || exit 1
)

libname=ACE
if [ "k$build" = "kDebug" ]; then
	libname=ACEd
fi

(
	ACE_DIR=`cygpath --mixed "$ACE_ROOT"`
	echo "export ACE_DIR='$ACE_DIR'"
	echo "export ACE_ROOT='$ACE_DIR'"
	echo "export ACE_LIBNAME='$libname'"
) > $BUILD_DIR/ace_${compiler}_${variant}_${build}.sh
