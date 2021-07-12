#!/bin/bash

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

set -e

# We test against multiple swig versions.  Where are they?
# You'll probably need to remove your global swig version to prevent
# cmake finding it later.
swig_base="/cache/swig"
if [ ! -e $swig_base ]; then
    swig_base="$PWD/cache/swig"
    mkdir -p $swig_base
fi

known_swig_versions="\
 1.3.37 1.3.38 1.3.39 1.3.40\
 2.0.0 2.0.1 2.0.2 2.0.3 2.0.4 2.0.5 2.0.6 2.0.7 2.0.8 2.0.9 2.0.10 2.0.11 2.0.12\
 3.0.0 3.0.1 3.0.2 3.0.3 3.0.4 3.0.5 3.0.6 3.0.7 3.0.8"

if [ -z "$SWIG_VERSIONS" ]; then
    swig_versions="$known_swig_versions"
else
    swig_versions="$SWIG_VERSIONS"
fi

SUPPORTED_LANGUAGES="PYTHON JAVA LUA RUBY CSHARP PERL TCL"


if [ "$1" = "" ]; then
    echo "Possible SWIG versions are $swig_versions"
    echo "Possible languages are $SUPPORTED_LANGUAGES"
    echo "To test a specific SWIG version / language combination, call as e.g.:"
    echo "  ./tests/integration/check-bindings.sh 2.0.11 LUA"
    echo "or to test many SWIG versions, call as:"
    echo "  ./tests/integration/check-bindings.sh LUA"
    echo "Before anything works you need to compile many SWIG versions:"
    echo "  create a directory /cache/swig writeable by you"
    echo "  get git repo https://github.com/swig/swig"
    echo "  then run \"..../tests/integration/check-bindings.sh swig\" from that directory"
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
    for swig_ver in $swig_versions; do
    echo " "
    echo "================================================================"
    echo "Working on $lang with $swig_ver"
    if [ ! -e "$swig_base/$swig_ver" ]; then
        echo "CANNOT FIND precompiled versions of swig."
        echo "In $PWD, looking in $swig_base/$sig_ver"
        echo "Going to compile it from scratch.  Warning: SLOW."
        echo "To cache this work, make a directory called '/cache/swig' that I can write in"
        base="$PWD"
        cd $swig_base
        if [ ! -e swig ]; then
            git clone https://github.com/swig/swig
        fi
        cd swig
        git clean -f -d
        git checkout .
        git checkout rel-$swig_ver
        ./autogen.sh
        ./configure --prefix=$swig_base/$swig_ver
        make clean
        make
        make install
    fi
    export SWIG_LIB=$swig_base/$swig_ver/share/swig/$swig_ver
    $swig_base/$swig_ver/bin/swig -swiglib
    search_path="-DSWIG_EXECUTABLE=$swig_base/$swig_ver/bin/swig -DSWIG_DIR=$swig_base/$swig_ver -DSWIG_VERSION=$swig_ver"
    cd $YARP_ROOT
    mkdir -p bindings
    cd bindings
    dir="check_$lang"
    rm -rf $dir
    mkdir -p $dir
    cd $dir
    echo "* In $PWD"
    ok=true
    YARP_PYTHON_FLAGS="-DCREATE_PYTHON_VERSION=2.7 -DPython_ADDITIONAL_VERSIONS=2.7" # Old swig versions don't work with new python
    lang_var=YARP_${lang}_FLAGS
    lang_flags=${!lang_var}
    echo "Running cmake"
    set +e
    rm -f CMakeCache.txt
    opt_flags=""
    if [[ "$TRAVIS_WITH_INTEGRATION_TESTS" ]]; then
        # turn off optimizations and force use of clang to fit into travis memory limits
        opt_flags="-DCMAKE_CXX_FLAGS='-O0' -DCMAKE_C_FLAGS='-O0' -DCMAKE_C_COMPILER='clang' -DCMAKE_CXX_COMPILER='clang++'"
    fi
    cmake -DCREATE_$lang=TRUE $opt_flags $lang_flags $search_path $YARP_ROOT/bindings > result.txt 2>&1
    set -e
    if [ ! -e CMakeCache.txt ] ; then
        cat result.txt
        exit 1
    fi
    if grep -q SWIG_EXECUTABLE CMakeCache.txt; then
        echo "Configured."
    else
        cat result.txt
        exit 1
    fi
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
    NPID=""
    if $YARP_DIR/bin/yarp where; then
        echo "name server present"
    else
        $YARP_DIR/bin/yarp namespace /bindings
        $YARP_DIR/bin/yarpserver &
        NPID="$!"
        $YARP_DIR/bin/yarp wait /bindings
    fi

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
    if [ ! "k$NPID" = "k" ]; then
        kill $NPID
    fi
    done
    echo $successes >> $log
    echo $failures >> $log
done
cat $log
if $failed; then
    exit 1
fi
