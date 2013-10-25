#!/bin/bash

set -e

# We test against multiple swig versions.  Where are they?
# You'll probably need to remove your global swig version to prevent
# cmake finding it later.
swig_base="/cache/swig"
swig_versions="1.3.39 1.3.40 2.0.1 2.0.2 2.0.3 2.0.4 2.0.5 2.0.6 2.0.7 2.0.8 2.0.9 2.0.10 2.0.11"

if [ "$1" = "swig" ]; then
    echo "Compiling all swig versions"
    if [ ! -e swig.spec.in ]; then
	echo "Please get swig git repo https://github.com/swig/swig and run from repo"
	exit 1
    fi
    for f in $swig_versions; do
	if [ ! -e $swig_base/$f ] ; then
	    echo $f
	    git clean -f -d
	    git checkout .
	    git checkout rel-$f
	    ./autogen.sh
	    ./configure --prefix=$swig_base/$f
	    make clean
	    make
	    make install
	fi
    done
    exit 0
fi


if [ "$YARP_ROOT" = "" ]; then
    YARP_ROOT="$PWD"
fi

if [ ! -e "$YARP_ROOT/conf/YarpVersion.cmake" ] ; then
    echo "Please run either from \$YARP_ROOT or with \$YARP_ROOT set"
    exit 1
fi

SUPPORTED_LANGUAGES="LUA PYTHON JAVA RUBY CSHARP PERL TCL"

echo $1 | grep -q "[.]" && {
    swig_versions=$1
    shift
}

if [ ! "$1" = "" ]; then
    SUPPORTED_LANGUAGES="$*"
fi

log=$PWD/check-bindings.txt
echo "Writing to log $log"
rm -f $log
touch $log
for lang in $SUPPORTED_LANGUAGES; do
    echo "Working on $lang"
    successes="$lang success: "
    failures="$lang failure: "
    for swig_ver in $swig_versions; do
	echo "Working on $lang with $swig_ver"
	search_path="-DCMAKE_SYSTEM_PROGRAM_PATH=/cache/swig/$swig_ver/bin -DCMAKE_SYSTEM_PREFIX_PATH=/cache/swig/$swig_ver"
	cd $YARP_ROOT
	mkdir -p bindings
	cd bindings
	dir="check_$lang"
	rm -rf $dir
	mkdir -p $dir
	cd $dir
	echo "* In $PWD"
	ok=true
	YARP_JAVA_FLAGS="-DPREPARE_CLASS_FILES=TRUE"
	YARP_PYTHON_FLAGS="-DCREATE_PYTHON_VERSION=2.7 -DPython_ADDITIONAL_VERSIONS=2.7" # Old swig versions don't work with new python
	lang_var=YARP_${lang}_FLAGS
	lang_flags=${!lang_var}
	cmake -DCREATE_$lang=TRUE $lang_flags $search_path $YARP_ROOT/bindings
	grep SWIG_EXECUTABLE CMakeCache.txt | grep "/cache/swig/$swig_ver/"
	make VERBOSE=1 || { 
	    echo "make failed for $lang $swig_ver" >> $log
	    ok=false
	}
	lc=`echo $lang | tr '[:upper:]' '[:lower:]'`
	run=$YARP_ROOT/bindings/tests/$lc/run.sh
	if [ -e $run ]; then
	    bash $run || {
		echo "runtime failed for $lang $swig_ver" >> $log
		ok=false
	    }
	fi
	if $ok; then
	    echo "success for $lang $swig_ver" >> $log
	    successes="$successes $swig_ver"
	else
	    failures="$failures $swig_ver"
	fi
    done
    echo $successes >> $log
    echo $failures >> $log
done
