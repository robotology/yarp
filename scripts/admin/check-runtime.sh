#!/bin/bash

if [ "k$TRAVIS_WITH_ACE" = "kfalse" ] ; then
    echo Nothing to check yet without ACE
    exit 0
fi

function header() {
    echo "###############################################################"
    echo "## $1"
}

function fail() {
    echo "Failure: $1"
    exit 1
}

set -e
echo "This script runs some tests that are hard to do in the regular harness"

if [ ! -e CMakeCache.txt ] ; then
    echo "Please run from build directory"
    exit 1
fi

src=`grep YARP_SOURCE_DIR CMakeCache.txt | sed "s/.*=//"`

base="$PWD/runtime_tests"

mkdir -p $base
cd $base
mkdir -p $base/fakebot
mkdir -p $base/fakebot_static

PLUGIN_FLAGS="-DCREATE_DEVICE_LIBRARY_MODULES=TRUE -DENABLE_yarpmod_fakebot=TRUE -DCREATE_OPTIONAL_CARRIERS=TRUE -DENABLE_yarpcar_human_carrier=TRUE"

# Create fakebot device
cd $base/fakebot
echo "Working in $PWD"
if [ ! -e CMakeCache.txty ]; then
    cmake -DCMAKE_INSTALL_PREFIX=$base/root $PLUGIN_FLAGS $src
fi
make
make install

cd $base/fakebot_static
echo "Working in $PWD"
if [ ! -e CMakeCache.txt ]; then
    cmake -DCMAKE_INSTALL_PREFIX=$base/root_static $PLUGIN_FLAGS -DCREATE_SHARED_LIBRARY=FALSE $src
fi
make
make install

cd $base
export YARP_CONF=$PWD
echo "0 0 local" > yarp.conf
export YARP_DATA_DIRS=$PWD/fakebot/share/yarp 

######################################################################
## Shared version of plugins

yarp="./fakebot/bin/yarp"

header "Check shared fakebot is findable"
$yarp plugin | grep fakebot || fail "Could not find fakebot"

header "Check shared fakebot is startable"
${yarp}dev --device fakebot --lifetime 1 || fail "Could not start fakebot"

header "Check shared human carrier will run"
$yarp read /localhost:10111 &
echo "Hello" | $yarp write /localhost:10112 --wait-connect &
$yarp wait /localhost:10111
$yarp wait /localhost:10112
$yarp connect /localhost:10112 /localhost:10111 fakehuman && fail "fakehuman should not work" || echo ok
$yarp connect /localhost:10112 /localhost:10111 human
# human is a very very odd and old carrier
$yarp terminate /localhost:10111 || echo ok
$yarp terminate /localhost:10112 || echo ok
$yarp terminate /localhost:10111 || echo ok
$yarp terminate /localhost:10112 || echo ok

header "Check shared fakebot is unfindable without YARP_DATA_DIRS"
export YARP_DATA_DIRS=$PWD/share/notyarpatall
$yarp plugin | grep fakebot && fail "Should not have found fakebot" || echo "not found, good"

header "Check shared fakebot is startable in build without YARP_DATA_DIRS"
cd fakebot
./bin/yarpdev --device fakebot --lifetime 1 || fail "Could not start fakebot"

######################################################################
## Static version of plugins

cd $base
yarp="./fakebot_static/bin/yarp"

header "Check static fakebot is startable"
${yarp}dev --device fakebot --lifetime 1 || fail "Could not start fakebot"

header "Check static human carrier will run"
$yarp read /localhost:10111 &
echo "Hello" | $yarp write /localhost:10112 --wait-connect &
$yarp wait /localhost:10111
$yarp wait /localhost:10112
$yarp connect /localhost:10112 /localhost:10111 fakehuman && fail "fakehuman should not work" || echo ok
$yarp connect /localhost:10112 /localhost:10111 human
# human is a very very odd and old carrier
$yarp terminate /localhost:10111 || echo ok
$yarp terminate /localhost:10112 || echo ok
$yarp terminate /localhost:10111 || echo ok
$yarp terminate /localhost:10112 || echo ok


######################################################################
## Installed shared version of plugins

cd $base
export YARP_DATA_DIRS=$PWD/root/share/yarp 
export LD_LIBRARY_PATH=$PWD/root/lib

header "Check installed shared fakebot is startable"

$base/root/bin/yarpdev --device fakebot --lifetime 1 || fail "Could not start fakebot"

######################################################################
## Installed static version of plugins

cd $base
export YARP_DATA_DIRS=nothing
export LD_LIBRARY_PATH=nothing

header "Check installed static fakebot is startable"

$base/root_static/bin/yarpdev --device fakebot --lifetime 1 || fail "Could not start fakebot"

header "Done, no problems found"

