#!/bin/bash

set -e

# We test against multiple swig versions.  Where are they?
# You'll probably need to remove your global swig version to prevent
# cmake finding it later.
swig_base="/cache/swig"
if [ -e "cache/swig" ]; then
    swig_base="$PWD/cache/swig"
fi

# swig_versions="1.3.39 1.3.40 2.0.1 2.0.2 2.0.3 2.0.4 2.0.5 2.0.6 2.0.7 2.0.8 2.0.9 2.0.10 2.0.11 2.0.12 3.0.0 3.0.1 3.0.2"
default_swig_versions="1.3.39 1.3.40 2.0.2 2.0.5 2.0.6 2.0.10 2.0.12 3.0.0 3.0.1 3.0.2"
if [ -z "$SWIG_VERSIONS" ]; then
swig_versions="$default_swig_versions"
else
swig_versions="$SWIG_VERSIONS"
fi

skip_versions="RUBY_1.3.39 RUBY_1.3.40 LUA_2.0.11"

SUPPORTED_LANGUAGES="PYTHON JAVA LUA RUBY CSHARP PERL TCL"

if [ "$1" = "" ]; then
    echo "Possible SWIG versions are $swig_versions"
    echo "Possible languages are $SUPPORTED_LANGUAGES"
    echo "To test a specific SWIG version / language combination, call as e.g.:"
    echo "  ./scripts/admin/check-bindings.sh 2.0.11 LUA"
    echo "or to test many SWIG versions, call as:"
    echo "  ./scripts/admin/check-bindings.sh LUA"
    echo "Before anything works you need to compile many SWIG versions:"
    echo "  create a directory /cache/swig writeable by you"
    echo "  get git repo https://github.com/swig/swig"
    echo "  then run \"..../scripts/admin/check-bindings.sh swig\" from that directory"
    echo "  you should end up with lots of swig version in /cache/swig/N.N.N"
    exit 0
fi

if [ "$1" = "swig" ]; then
    echo "Compiling all swig versions"
    if [ ! -e swig.spec.in ]; then
	echo "Please get swig git repo https://github.com/swig/swig and run this script from the root directory of that repo"
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
failed=false
for lang in $SUPPORTED_LANGUAGES; do
    successes="$lang success: "
    failures="$lang failure: "
    skips="$lang skips: "
    for swig_ver in $swig_versions; do
	echo " "
	echo "================================================================"
	echo "Working on $lang with $swig_ver"
	echo " $skip_versions " | grep " ${lang}_${swig_ver} " && skip=true || skip=false
	if $skip; then
	    echo "Skipping $lang $swig_ver"
	    skips="$skips $swig_ver"
	    continue
	fi
	export SWIG_LIB=$swig_base/$swig_ver/share/swig/$swig_ver
	$swig_base/$swig_ver/bin/swig -swiglib
	search_path="-DCMAKE_SYSTEM_PROGRAM_PATH=$swig_base/$swig_ver/bin -DCMAKE_SYSTEM_PREFIX_PATH=$swig_base/$swig_ver"
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
	echo "Running cmake"
	cmake -DCREATE_$lang=TRUE $lang_flags $search_path $YARP_ROOT/bindings > result.txt 2>&1
	grep SWIG_EXECUTABLE CMakeCache.txt | grep "$swig_base/$swig_ver/" || {
	    echo "CMake is picking up wrong SWIG version"
	    echo "Remove this: `grep SWIG_EXECUTABLE CMakeCache.txt`"
	    exit 1
	}
	echo "Running make"
	{
	    make VERBOSE=1 || { 
		echo "make failed for $lang $swig_ver" >> $log
		ok=false
	    }
	} >> result.txt 2>&1
	if $ok; then
	    echo "make succeeded for $lang $swig_ver"
	else
	    cat result.txt
	fi
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
	    failed=true
	fi
    done
    echo $successes >> $log
    echo $failures >> $log
    echo $skips >> $log
done
cat $log
if $failed; then
    exit 1
fi
