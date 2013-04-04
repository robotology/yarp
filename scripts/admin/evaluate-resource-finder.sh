#!/bin/bash

# Establish a yarp binary to test with
yarp="$PWD/bin/yarp"
if [ ! "k$1" = "k" ]; then
    yarp="$1"
fi
if [ ! -e "$yarp" ]; then
    echo "Cannot find yarp executable $yarp"
    echo "Please supply full path as argument"
fi

# Establish a test directory
base=/tmp/rf_test
rm -rf $base
mkdir $base || exit 1
cd $base || exit 1

# Override all basic search locations
export YARP_DATA_HOME=$base/home/yarper/.local/share/yarp

export YARP_CONFIG_HOME=$base/home/yarper/.config/yarp

export YARP_DATA_DIR0=$base/usr/share/yarp
export YARP_DATA_DIR1=$base/usr/local/share/yarp
export YARP_DATA_DIRS=$YARP_DATA_DIR0:$YARP_DATA_DIR1

export YARP_CONFIG_DIR0=$base/etc/yarp
export YARP_CONFIG_DIRS=$YARP_CONFIG_DIR0

mkdir -p $YARP_DATA_HOME
mkdir -p $YARP_CONFIG_HOME
mkdir -p ${YARP_DATA_DIRS//:/ }
mkdir -p ${YARP_CONFIG_DIRS//:/ }

echo "=============================================================================="
echo "= Overriding all basic search locations"
echo "= YARP_DATA_HOME=$YARP_DATA_HOME"
echo "= YARP_CONFIG_HOME=$YARP_CONFIG_HOME"
echo "= YARP_DATA_DIRS=$YARP_DATA_DIRS"
echo "= YARP_CONFIG_DIRS=$YARP_CONFIG_DIRS"
echo ""

# Add some packaged directories using path.d mechanism
mkdir -p $YARP_CONFIG_DIR0/path.d
{
cat<<EOF
[search icub]
path $base/usr/share/iCub
EOF
} > $YARP_CONFIG_DIR0/path.d/icub.ini
{
cat<<EOF
[search steampunk]
path $base/usr/share/steampunk
EOF
} > $YARP_CONFIG_DIR0/path.d/steampunk.ini


echo "Using $yarp"

echo "Trying a search for a non-existent file, probe.ini"
$yarp resource --policy none --find probe.ini


