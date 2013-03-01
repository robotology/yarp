#!/bin/bash

# Actually, we just download a precompiled gtkmm.
# We could compile it, but it doesn't seem like we need to do better distributing it
# than what already exists.

BUILD_DIR=$PWD

source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

source $SETTINGS_BUNDLE_FILENAME || {
	echo "Bundle settings not found"
	exit 1
}

source $SETTINGS_SOURCE_DIR/src/process_options.sh $* || {
	echo "Cannot process options"
	exit 1
}

if [ "k$BUNDLE_GTKMM_VERSION" = "k" ]; then
	#BUNDLE_GTKMM_VERSION=2.22.0-2
	echo "Set GTKMM version"
	exit 1
fi

GTKMM_PLATFORM=""
ext=exe
if [ "$OPT_VARIANT" == "x86" ] ; then
	GTKMM_PLATFORM="win32"
	if [ "$BUNDLE_GTKMM_32_ZIP" != "" ] ; then
		ext=zip
	fi
elif [ "$OPT_VARIANT" == "x64" ] || [ "$OPT_VARIANT" == "amd64" ] || [ "$OPT_VARIANT" == "x86_amd64" ] ; then
	GTKMM_PLATFORM="win64"
	if [ "$BUNDLE_GTKMM_64_ZIP" != "" ] ; then
		ext=zip
	fi
else
	echo "Unsupported GTKMM platform $OPT_VARIANT"
	exit 1
fi
fname=gtkmm-${GTKMM_PLATFORM}-devel-$BUNDLE_GTKMM_VERSION

if [ "$ext" = "exe" ]; then

  if [ ! -e $fname.exe ]; then
	GTKMM_DIR=`echo $BUNDLE_GTKMM_VERSION | sed "s/\.[-0-9]*$//"`
	wget -O $fname.exe http://ftp.gnome.org/pub/GNOME/binaries/${GTKMM_PLATFORM}/gtkmm/$GTKMM_DIR/${fname}.exe || (
		echo "Cannot fetch GTKMM"
		rm -f $fname.exe
		exit 1
	)
  fi

  if [ ! -d $fname ]; then
	mkdir -p $fname
	cd $fname
	7z x ../$fname.exe || {
		echo "Cannot unpack GTKMM"
		cd $BUILD_DIR
		rm -rf $fname
		exit 1
	}
  fi

  if [ ! -e $BUILD_DIR/$fname/include ]; then
	# rejigger files for Lorenzo
	cd $BUILD_DIR/$fname
	mkdir -p include || exit 1
	cd include || exit 1
	ALT=../*_OUTDIR
	for d in gtk-2.0 cairo glib-2.0 pango-1.0 atk-1.0 glib-2.0 gtk-2.0 gdk-pixbuf-2.0; do
		echo "Copying $d to include"
		if [ ! -e $d ]; then
			cp -R $ALT/$d $d || exit 1
		fi
	done
	mkdir -p ../lib/glib-2.0/include
	cp $ALT/include/glibconfig.h ../lib/glib-2.0/include || exit 1
	mkdir -p ../lib/gtk-2.0/include
	cp $ALT/include/gdkconfig.h ../lib/gtk-2.0/include || exit 1
  fi
fi

GTKMM_PATH=""

if [ "$ext" = "zip" ]; then

  if [ ! -e $fname.zip ]; then
    if [ "$OPT_VARIANT" == "x86" ] ; then
		wget -O $fname.zip $BUNDLE_GTKMM_32_ZIP || {
			echo "Cannot fetch GTKMM"
			rm -f $fname.zip
			exit 1
		}
		GTKMM_PATH="gtkmm"
	elif [ "$OPT_VARIANT" == "x64" ] || [ "$OPT_VARIANT" == "amd64" ] || [ "$OPT_VARIANT" == "x86_amd64" ] ; then
		wget -O $fname.zip $BUNDLE_GTKMM_64_ZIP || {
			echo "Cannot fetch GTKMM"
			rm -f $fname.zip
			exit 1
		}
		GTKMM_PATH="gtkmm64"
	else
		echo "Unsupported GTKMM platform $OPT_VARIANT"
		exit 1
	fi
  fi

  if [ ! -d $fname ] ; then
    if [ ! -e "$GTKMM_PATH" ] ; then
      unzip $fname.zip || {
	  	echo "Cannot unpack GTKMM"
		cd $BUILD_DIR
		rm -rf $GTKMM_PATH
		exit 1
	  }
	  
	  ## we follow instructions in gtkmm/redist/README
	  cd $GTKMM_PATH
	  mv bin bin-backup
	  mkdir bin
	  cp redist/*.dll bin/
	  
	  cd bin
	  rm libsigc-*.dll libglibmm-*.dll libgiomm-*.dll libcairomm-*.dll libpangomm-*.dll libatkmm-*.dll libgdkmm-*.dll libgtkmm-*.dll libxml++-*.dll libglademm-*.dll;

	  cd ..
	  cp bin-backup/*vc[0-9]0*.dll bin
	  rm bin-backup -rf
	  
	  cd ..
	fi
	#mv $fname.zip store-$fname.zip
	#mv $fname.exe store-$fname.exe
	sleep 3
	mv $GTKMM_PATH $fname || exit 1
	#mv store-$fname.zip $fname.zip
	#mv store-$fname.exe $fname.exe
  fi
fi


(
	GTKMM_DIR=`cygpath --mixed "$BUILD_DIR/$fname"`
	echo "export GTKMM_DIR='$GTKMM_DIR'"
	echo "export GTK_BASEPATH='$GTKMM_DIR'"
	echo "export GTKMM_BASEPATH='$GTKMM_DIR'"
) > $BUILD_DIR/gtkmm_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh
