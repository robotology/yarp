#!/bin/bash

##############################################################################
#
# Copyright: (C) 2011 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
# Amalgamate builds into an NSIS package
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
	exit 1
}

# GET OPT_* variables (build options) by processing our command-line options
source $SETTINGS_SOURCE_DIR/src/process_options.sh $* || {
	echo "Cannot process options"
	exit 1
}

# Figure out whether package will compile debug and release builds in one
if [ "k$OPT_BUILD" = "kany" ]; then
	base_build="Release"
	add_debug=true
else
	base_build="$OPT_BUILD"
	add_debug=false
fi
if $OPT_GCCLIKE; then
	add_debug=false
fi

# If we are combining debug build with release, we need to load and
# rename ACE and YARP paths for those builds
if $add_debug; then
	source ace_${OPT_COMPILER}_${OPT_VARIANT}_Debug.sh || {
		echo "Cannot find corresponding ACE debug build"
		exit 1
	}
	ACE_DIR_DBG="$ACE_DIR"
	ACE_ROOT_DBG="$ACE_ROOT"
	ACE_LIBNAME_DBG="$ACE_LIBNAME"

	source yarp_${OPT_COMPILER}_${OPT_VARIANT}_Debug.sh || {
		echo "Cannot find corresponding YARP debug build"
		exit 1
	}
	YARP_DIR_DBG="$YARP_DIR"
	YARP_ROOT_DBG="$YARP_ROOT"
fi

# Pick up ACE paths
source ace_${OPT_COMPILER}_${OPT_VARIANT}_${base_build}.sh || {
	echo "Cannot find corresponding ACE build"
	exit 1
}

# Pick up YARP paths
source yarp_${OPT_COMPILER}_${OPT_VARIANT}_${base_build}.sh || {
	echo "Cannot find corresponding YARP build"
	exit 1
}

# Pick up GTKMM paths
source gtkmm_${OPT_COMPILER}_${OPT_VARIANT}_${base_build}.sh || {
	echo "Cannot find corresponding GTKMM build"
	exit 1
}

# Pick up NSIS paths
source nsis_any_any_any.sh || {
	echo "Cannot find corresponding NSIS build"
	exit 1
}

# Make build directory
fname=yarp_core_package-$BUNDLE_YARP_VERSION
fname2=$fname-$OPT_COMPILER-$OPT_VARIANT-$OPT_BUILD
mkdir -p $fname2
cd $fname2 || exit 1
OUT_DIR=$PWD

# Clear any zip-related material
zip_name="$fname2"
rm -rf "$OUT_DIR/*_zip.sh"

# Function to prepare stub files for adding/removing files for an NSIS
# section, and for building the corresponding zip file
function nsis_setup {
	prefix=$1
	echo -n > ${OUT_DIR}/${prefix}_add.nsi
	echo -n > ${OUT_DIR}/${prefix}_remove.nsi
	echo -n > ${OUT_DIR}/${prefix}_zip.sh
}

# Add a file or files into list to be added/removed from NSIS section,
# and to be placed into the corresponding zip file.  Implementation is
# complicated by the need to avoid calling the super-slow cygpath
# command too often.
CYG_BASE=`cygpath -w /`
function nsis_add_base {
	mode=$1
	prefix=$2
	src=$3
	dest=$4
	dir=$5 #optional
	echo "Add " "$@"
	osrc="$src"
	odest="$dest"
	if [ "k$dir" = "k" ] ; then
		src="$PWD/$src"
		osrc="$src"
		src=${src//\//\\}
		src="$CYG_BASE$src"
	else
		src="$dir/$src"
		osrc="$src"
		src=${src//\//\\}
	fi
	dest=${dest//\//\\} # flip to windows convention
	zodest1="zip/$prefix/$zip_name/$odest"
	zodest2="zip_all/$zip_name/$odest"
	if [ "$mode" = "single" ]; then
		echo "File /oname=$dest $src" >> $OUT_DIR/${prefix}_add.nsi
		echo "Delete \"\$INSTDIR\\$dest\"" >> $OUT_DIR/${prefix}_remove.nsi
		echo "mkdir -p `dirname $zodest1`" >> $OUT_DIR/${prefix}_zip.sh
		echo "mkdir -p `dirname $zodest2`" >> $OUT_DIR/${prefix}_zip.sh
		echo "cp '$osrc' $zodest1" >> $OUT_DIR/${prefix}_zip.sh
		echo "cp '$osrc' $zodest2" >> $OUT_DIR/${prefix}_zip.sh
	else
		# recursive
		echo "SetOutPath \"\$INSTDIR\\$dest\..\"" >> $OUT_DIR/${prefix}_add.nsi
		echo "File /r $src" >> $OUT_DIR/${prefix}_add.nsi
		echo "RmDir /r \"\$INSTDIR\\$dest\"" >> $OUT_DIR/${prefix}_remove.nsi
		echo "mkdir -p `dirname $zodest1`" >> $OUT_DIR/${prefix}_zip.sh
		echo "mkdir -p `dirname $zodest2`" >> $OUT_DIR/${prefix}_zip.sh
		echo "cp -r '$osrc' $zodest1" >> $OUT_DIR/${prefix}_zip.sh
		echo "cp -r '$osrc' $zodest2" >> $OUT_DIR/${prefix}_zip.sh
	fi
}


# Add a single file into list to be added/removed from NSIS section,
# and to be placed into the corresponding zip file.
function nsis_add {
	nsis_add_base single "$@"
}

# Add a directory to be added/removed from NSIS section.
function nsis_add_recurse {
	nsis_add_base recurse "$@"
}

# Get base YARP path in unix format
YARP_DIR_UNIX=`cygpath -u $YARP_DIR`
if $add_debug; then
	YARP_DIR_DBG_UNIX=`cygpath -u $YARP_DIR_DBG`
fi

# Set up stubs for all NSIS sections
nsis_setup yarp_base
nsis_setup yarp_libraries
nsis_setup yarp_dlls
nsis_setup yarp_headers
nsis_setup yarp_programs
nsis_setup yarp_math_libraries
nsis_setup yarp_math_dlls
nsis_setup yarp_math_headers
nsis_setup yarp_guis
nsis_setup yarp_ace_libraries
nsis_setup yarp_ace_dlls
nsis_setup yarp_vc_dlls
nsis_setup yarp_examples
nsis_setup yarp_debug

# GSL is currently compiled statically with yarp_math, so don't need:
# nsis_setup yarp_gsl_libraries
# nsis_setup yarp_gsl_dlls

# Add YARP material to NSIS
cd $YARP_DIR_UNIX || exit 1
YARP_LICENSE="$YARP_ROOT/LGPL.txt"
nsis_add yarp_base YARPConfigForInstall.cmake YARPConfig.cmake
cd $YARP_DIR_UNIX/install || exit 1
nsis_add_recurse yarp_base share share
cd $YARP_DIR_UNIX/install/lib || exit 1
for d in `ls -1 -d --group-directories-first YARP-* | head`; do
	nsis_add_recurse yarp_base $d lib/$d
	YARP_LIB_DIR="$d"
	YARP_LIB_FILE=`cd $d; ls YARP-*.cmake`
done
cd $YARP_DIR_UNIX/install/lib || exit 1
for f in `ls -1 *.$LIBEXT | grep -v YARP_math`; do
	nsis_add yarp_libraries $f lib/$f
done
for f in `ls -1 *.$LIBEXT | grep YARP_math`; do
	nsis_add yarp_math_libraries $f lib/$f
done
for f in `ls -1 *.dll | grep -v YARP_math`; do
	nsis_add yarp_dlls $f bin/$f
done
for f in `ls -1 *.dll | grep YARP_math`; do
	nsis_add yarp_math_dlls $f bin/$f
done
cd $YARP_DIR_UNIX/install/bin
for f in `ls -1 *.exe | grep -v yarpview`; do
	nsis_add yarp_programs $f bin/$f
done
cd $YARP_ROOT/example/hello
for f in `ls *.cpp CMakeLists.txt`; do
	nsis_add yarp_examples $f example/$f
done
cd $YARP_DIR_UNIX/install/bin
for f in `ls -1 *.exe | grep yarpview`; do
	nsis_add yarp_guis $f bin/$f
done
cd $YARP_DIR_UNIX/install/include/yarp
for f in conf os sig dev ; do
	nsis_add_recurse yarp_headers $f include/yarp/$f
done
nsis_add_recurse yarp_math_headers math include/yarp/math

# Add GTKMM material to NSIS
if [ "k$SKIP_GTK" = "k" ]; then
	cd $GTKMM_DIR/redist || exit 1
    for f in `ls *.dll`; do
		chmod u+x $f
		nsis_add yarp_guis $f bin/$f
	done
	cd $GTKMM_DIR/bin || exit 1
	for f in zlib1.dll freetype6.dll intl.dll; do
		chmod u+x $f
		nsis_add yarp_guis $f bin/$f
	done
fi

# Add ACE material to NSIS
cd $ACE_DIR || exit 1
cd lib || exit 1
if [ ! -e $ACE_LIBNAME.dll ]; then
	echo "Cannot find $ACE_LIBNAME.dll in $PWD"
	exit 1
fi
nsis_add yarp_ace_libraries $ACE_LIBNAME.$LIBEXT lib/$ACE_LIBNAME.$LIBEXT
nsis_add yarp_ace_dlls $ACE_LIBNAME.dll bin/$ACE_LIBNAME.dll

# Add Visual Studio redistributable material to NSIS
if [ -e "$OPT_VC_REDIST_CRT" ] ; then
	cd "$OPT_VC_REDIST_CRT" || exit 1
	for f in `ls *.dll *.manifest`; do
		nsis_add yarp_vc_dlls $f bin/$f "$OPT_VC_REDIST_CRT"
	done
fi

# Add debug material to NSIS
DBG_HIDE="-"
if $add_debug; then
	cd $YARP_DIR_DBG_UNIX/install/lib || exit 1
	for f in `ls -1 *.lib`; do
		nsis_add yarp_debug $f lib/$f
	done
	cd $YARP_DIR_DBG_UNIX/install/lib || exit 1
	for f in `ls -1 *.dll`; do
		nsis_add yarp_debug $f bin/$f
	done
	cd $ACE_DIR_DBG || exit 1
	cd lib || exit 1
	if [ ! -e $ACE_LIBNAME_DBG.dll ]; then
		echo "Cannot find $ACE_LIBNAME_DBG.dll in $PWD"
		exit 1
	fi
	nsis_add yarp_debug $ACE_LIBNAME_DBG.$LIBEXT lib/$ACE_LIBNAME_DBG.$LIBEXT
	nsis_add yarp_debug $ACE_LIBNAME_DBG.dll bin/$ACE_LIBNAME_DBG.dll
	cd $YARP_DIR_DBG_UNIX/install/lib || exit 1
	for d in `ls -1 -d --group-directories-first YARP-* | head`; do
		YARP_LIB_DIR_DBG="$d"
		YARP_LIB_FILE_DBG=`cd $d; ls YARP-*.cmake`
		nsis_add yarp_debug $YARP_LIB_DIR_DBG/$YARP_LIB_FILE_DBG lib/$YARP_LIB_DIR_DBG/$YARP_LIB_FILE_DBG
	done
	DBG_HIDE=""
fi


# Run NSIS
cd $OUT_DIR
cp $SETTINGS_SOURCE_DIR/src/nsis/*.nsh .
$NSIS_BIN -DYARP_VERSION=$BUNDLE_YARP_VERSION -DBUILD_VERSION=${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD} -DYARP_LICENSE=$YARP_LICENSE -DYARP_ORG_DIR=$YARP_DIR -DACE_ORG_DIR=$ACE_DIR -DYARP_LIB_DIR=$YARP_LIB_DIR -DYARP_LIB_FILE=$YARP_LIB_FILE -DDBG_HIDE=$DBG_HIDE -DYARP_ORG_DIR_DBG=$YARP_DIR_DBG -DACE_ORG_DIR_DBG=$ACE_DIR_DBG -DYARP_LIB_DIR_DBG=$YARP_LIB_DIR_DBG -DYARP_LIB_FILE_DBG=$YARP_LIB_FILE_DBG -DNSIS_OUTPUT_PATH=`cygpath -w $PWD` `cygpath -m $SETTINGS_SOURCE_DIR/src/nsis/yarp_core_package.nsi` || exit 1

# Generate zip files
if [ "k$SKIP_ZIP" = "k" ] ; then
	# flush zips
	rm -rf "$OUT_DIR/zip"
	rm -rf "$OUT_DIR/zip_all"

	for f in `ls *_zip.sh`; do
		echo "Processing $f"
		bash ./$f || exit 1
	done
	for f in `cd zip; ls`; do
		cd $OUT_DIR/zip/$f || exit 1
		echo "  Zipping $f"
		zip -r ${zip_name}-$f.zip $zip_name || exit 1
		mv *.zip $OUT_DIR || exit 1
	done
	echo "Big zip"
	cd $OUT_DIR/zip_all || exit 1
	zip -r ${zip_name}.zip ${zip_name} || exit 1
	mv *.zip $OUT_DIR || exit 1
fi

# Cache paths and variables, for dependent packages to read
cd $OUT_DIR
(
	echo "export YARP_CORE_PACKAGE_DIR='$PWD'"
) > $BUILD_DIR/yarp_core_package_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh

