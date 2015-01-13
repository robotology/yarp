#!/bin/bash

# Copyright: (C) 2013 iCub Facility
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if [ ! "k$YARP_DIR" = "k" ] ; then
    cd $YARP_DIR
fi

if [ ! -e CMakeCache.txt ] ; then
    echo "Run from build directory or with YARP_DIR set to build directory"
    exit 1
fi

YARP_BIN="$PWD/bin"

YARP_SRC=`grep YARP_SOURCE_DIR CMakeCache.txt | sed "s/.*=//"`
YARP_DIR="$PWD"
export YARP_DATA_DIRS=$YARP_DIR

echo "Run some basic ROS tests, assuming a ros install"
echo "Also assumes that YARP has been configured for ROS"

BASE=$PWD/check_ros_

########################################################################
# Some utilities and cleanup code

# Print a header for the test
function header {
echo " "
echo "====================================================================="
echo "== $1"
echo " "
}

function wait_file {
    fname="$1"
    while [ "`cat $fname | wc -c`" = "0" ] ; do
	echo "waiting for $fname"
	sleep 1
    done
}

function wait_node_topic {
    node="$1"
    topic="$2"
    rostopic info $topic | grep $node
    while [ "`rostopic info $topic | grep $node | wc -c`" = "0" ] ; do
	echo "waiting for $node on $topic"
	sleep 1
    done
}

function wait_topic_gone {
    topic="$1"
    while rostopic info $topic ; do
	echo "waiting for $topic to disappear"
	sleep 1
    done
}

function wait_node {
    node="$1"
    key="$2"
    while [ "`rosnode list | grep $node | wc -c`" = "0" ] ; do
	echo "waiting for $node"
	sleep 1
    done
    while [ "`rosnode info $node | grep $key | wc -c`" = "0" ] ; do
	echo "waiting for $node with key $key"
	sleep 1
    done
}

# Track PIDs of some processes we'll be running, for cleanup purposes
SERVER_ID=""
HELPER_ID=""
ROS_ID=""

cleanup_helper() {
    if [ ! "k$HELPER_ID" = "k" ]; then
	echo "Removing helper"
	kill $HELPER_ID || true
	wait $HELPER_ID || true
	HELPER_ID=""
    fi
}

cleanup_all() {
    cleanup_helper
    if [ ! "k$SERVER_ID" = "k" ]; then
	echo "Removing yarp server"
	kill $SERVER_ID || true
	wait $SERVER_ID || true
	SERVER_ID=""
    fi
    if [ ! "k$ROS_ID" = "k" ]; then
	echo "Removing ros server"
	kill $ROS_ID || true
	wait $ROS_ID || true
	ROS_ID=""
    fi
}
trap cleanup_all EXIT


########################################################################
header "Start name servers if needed"

rostopic list || {
    echo "String roscore"
    roscore > /dev/null &
    ROS_ID=$!
}

while ! rostopic list; do
    echo "Waiting for roscore"
    sleep 1
done

${YARP_BIN}/yarp where || {
    echo "Starting yarpserver"
    ${YARP_BIN}/yarpserver --ros --write > /dev/null &
    SERVER_ID=$!
}

while ! ${YARP_BIN}/yarp detect --write; do
    echo "Waiting for yarpserver"
    sleep 1
done

echo "roscore is OK"
echo "yarpserver is OK"

########################################################################
header "Check name server is found"

${YARP_BIN}/yarp where || exit 1

########################################################################
header "Test name gets listed"

${YARP_BIN}/yarp read /test/msg@/test_node &
HELPER_ID=$!

wait_node_topic /test_node /test/msg

cleanup_helper

echo "Topic should now be gone"
rostopic info /test_msg && exit 1 || echo "(this is correct)."

########################################################################
header "Test bag file record and playback"

root="/test/bag/pid$$"
rm -rf ${BASE}check_bag
mkdir -p ${BASE}check_bag
pushd ${BASE}check_bag
# rosbag record --output-name=log.bag $root/str &
cmake $YARP_SRC/tests/integration/ros/ -DYARP_DIR=$YARP_DIR
make

rosrun rosbag record -a -O log.bag &
HELPER_ID=$!
sleep 5
./test_topic $root wait

echo "Stopping rosbag process"
kill -s SIGINT $HELPER_ID
wait $HELPER_ID || echo ok
HELPER_ID=""
test log.bag

echo "Topic should now be gone"
rostopic info $root/str && exit 1 || echo "(this is correct)."

$YARP_BIN/yarp read $root/str@$root/reader > replay.txt &
HELPER_ID=$!
rosbag play log.bag --topics $root/str
cleanup_helper

grep "hello world" replay.txt || {
    echo "Failed to playback text"
    exit 1
}

rostopic echo $root/img -n 1 > replay_image.txt &
HELPER_ID=$!
rosbag play log.bag --topics $root/img
cleanup_helper

grep "is_bigendian" replay_image.txt || {
    echo "Failed to playback image"
    exit 1
}

rostopic echo $root/disp -n 1 > replay_disp.txt &
HELPER_ID=$!
rosbag play log.bag --topics $root/disp
cleanup_helper

grep "min_disparity" replay_disp.txt || {
    echo "Failed to playback disparity"
    exit 1
}

rostopic echo $root/cloud -n 1 > replay_cloud.txt &
HELPER_ID=$!
rosbag play log.bag --topics $root/cloud
cleanup_helper

grep "is_dense" replay_cloud.txt || {
    echo "Failed to playback pointcloud"
    exit 1
}


popd

########################################################################
header "Test yarp write name gets listed with right type"

typ="test_write/pid$$"
topic="/test/msg/$typ"

# yes | ${YARP_BIN}/yarp write $topic@/test_node --type $typ &
HELPER_ID=$( { { yes 0<&4 & echo $! >&3 ; } 4<&0 | ${YARP_BIN}/yarp write $topic@/test_node --type $typ >/dev/null & } 3>&1 | head -1 )

wait_node_topic /test_node $topic
${YARP_BIN}/yarp wait $topic@/test_node

if [ ! "k`rostopic info $topic | grep 'Type:'`" = "kType: $typ" ]; then
    echo "Type problem:"
    rostopic info $topic
    kill $YPID
    wait $YPID
    echo "That is not right at all"
    exit 1
fi

cleanup_helper

while ${YARP_BIN}/yarp exists $topic@/test_node ; do
    echo "Waiting for port to disappear"
    sleep 1
done

echo "Topic should now be gone"
{
    rostopic info $topic && {
	echo Topic is incorrectly lingering
	exit 1
    }
} || echo "(this is correct)."


########################################################################
header "Test yarp read name gets listed with right type"

typ="test_read/pid$$"
topic="/test/msg/$typ"
${YARP_BIN}/yarp read $topic@/test_node --type $typ &
HELPER_ID=$!

wait_node_topic /test_node $topic

if [ ! "k`rostopic info $topic | grep 'Type:'`" = "kType: $typ" ]; then
    echo "Type problem:"
    rostopic info $topic
    kill $YPID
    wait $YPID
    echo "That is not right at all"
    exit 1
fi

cleanup_helper

echo "Topic should now be gone"
rostopic info $topic && exit 1 || echo "(this is correct)."


########################################################################
header "Test yarp read name gets listed with right type using twiddle"

typ="test_twiddle/pid$$"
topic="/test/msg/$typ"
${YARP_BIN}/yarp read $topic@/test_node~$typ &
HELPER_ID=$!

wait_node_topic /test_node $topic

if [ ! "k`rostopic info $topic | grep 'Type:'`" = "kType: $typ" ]; then
    echo "Type problem:"
    rostopic info $topic
    kill $YPID
    wait $YPID
    echo "That is not right at all"
    exit 1
fi

cleanup_helper

echo "Topic should now be gone"
rostopic info $topic && exit 1 || echo "(this is correct)."


########################################################################
header "Test against rospy_tutorials/listener"

rm -f ${BASE}listener.log
touch ${BASE}listener.log
stdbuf --output=L rosrun rospy_tutorials listener > ${BASE}listener.log &
HELPER_ID=$!

echo "Hello" | ${YARP_BIN}/yarp write /chatter@/ros/check/write --type std_msgs/String --wait-connect

wait_file ${BASE}listener.log
result=`cat ${BASE}listener.log | sed "s/.*I heard //"`

for f in `rosnode list | grep "^/listener"`; do
    echo $f
    rosnode kill $f
done

echo "Result is '$result'"
if [ ! "Hello" = "$result" ] ; then
    echo "That is not right."
    exit 1
fi

cleanup_helper

########################################################################
header "Test against rospy_tutorials/talker"

rm -f ${BASE}talker.log
touch ${BASE}talker.log

rosrun rospy_tutorials talker &
stdbuf --output=L ${YARP_BIN}/yarp read /chatter@/ros/check/read > ${BASE}talker.log &
HELPER_ID=$!

wait_file ${BASE}talker.log
result=`cat ${BASE}talker.log | head -n1 | sed "s/world .*/world/" | sed "s/[^a-z ]//g"`

for f in `rosnode list | grep "^/talker"`; do
    echo $f
    rosnode kill $f
done

cleanup_helper

echo "Result is '$result'"
if [ ! "hello world" = "$result" ] ; then
    echo "That is not right."
    echo "Full text:"
    cat ${BASE}talker.log
    exit 1
fi

########################################################################
header "Test against rospy_tutorials/add_two_ints_server"

rm -f ${BASE}add_two_ints_server.log
touch ${BASE}add_two_ints_server.log

rm -f rospy_tutorials_AddTwoInts

${YARP_BIN}/yarpidl_rosmsg --name /typ@/yarpros --web false &
HELPER_ID=$!
wait_node /yarpros /typ

rosrun rospy_tutorials add_two_ints_server &
wait_node /add_two_ints_server /add_two_ints
echo "10 20" | ${YARP_BIN}/yarp rpc /add_two_ints > ${BASE}add_two_ints_server.log

result=`cat ${BASE}add_two_ints_server.log | sed "s/.* //"`

for f in `rosnode list | grep "^/add_two_ints_server"`; do
    echo $f
    rosnode kill $f
done

cleanup_helper

echo "Result is '$result'"
if [ ! "30" = "$result" ] ; then
    echo "That is not right."
    exit 1
fi

########################################################################
header "Test yarp images arrive"

node="/test/image/node/pid$$"
typ="sensor_msgs/Image"
topic="/test/image/$typ/pid$$"
echo ${YARP_BIN}/yarpdev --device test_grabber --name $topic@$node --width 16 --height 8
${YARP_BIN}/yarpdev --device test_grabber --name $topic@$node --width 16 --height 8 &
HELPER_ID=$!

wait_node_topic $node $topic

if [ ! "k`rostopic info $topic | grep 'Type:'`" = "kType: $typ" ]; then
    echo "Type problem:"
    rostopic info $topic
    kill $YPID
    echo "That is not right at all"
    exit 1
fi

rostopic echo $topic -n 10 > ${BASE}image.log
height=`cat ${BASE}image.log | grep "height:" | head -n1 | sed "s/.* //"`
width=`cat ${BASE}image.log | grep "width:" | head -n1 | sed "s/.* //"`
echo "width x height = $width x $height"

if [ ! "$width x $height" = "16 x 8" ] ; then
    echo "Size is not right"
    kill $YPID
    wait $YPID
    exit 1
else
    echo "Size is correct"
fi
cleanup_helper

########################################################################
header "Test ros images arrive"

node="/test/rimage/node/pid$$"
typ="sensor_msgs/Image"
topic="/test/rimage/$typ/pid$$"

rm -f ${BASE}rimage.log
touch ${BASE}rimage.log
stdbuf --output=L ${YARP_BIN}/yarp read $topic@$node > ${BASE}rimage.log &
HELPER_ID=$!
wait_node_topic $node $topic
rostopic pub -f ${BASE}image.log $topic sensor_msgs/Image || echo ok

wait_file ${BASE}rimage.log

grep "\[mat\] \[rgb\]" ${BASE}rimage.log && echo "(got an image, good)" || {
    echo "did not get an image"
    kill $YPID
    wait $YPID
    exit 1
}
cleanup_helper


########################################################################
header "Check MD5 checksums"

rm -rf ${BASE}_md5
mkdir -p ${BASE}_md5
pushd ${BASE}_md5

good=0
bad=0
rm -f good.txt
rm -f bad.txt
touch good.txt
touch bad.txt

for msg in `rosmsg list`; do
	$YARP_BIN/yarpidl_rosmsg $msg 2> /dev/null
	v1=`rosmsg md5 $msg`
	v2=$(grep md5 `echo $msg | sed "s|/|_|g"`.h | sed 's|.*Value."||' | sed 's|".*||')
	ok=0
	if [ "k$v1" = "k$v2" ]; then
		let good=$good+1
		echo $msg >> ${BASE}_md5_good.txt
		ok=1
	else
		let bad=$bad+1
		echo $msg >> ${BASE}_md5_bad.txt
	fi
	echo "$v1 $v2 $ok $msg"
done
echo "score good $good bad $bad"

if [ ! "$bad" = "0" ]; then
    echo "FAILURE of md5 for these types:"
    cat bad.txt
    exit 1
fi

if [ "$good" = "0" ]; then
    echo "Messages not found"
    exit 1
fi

echo "All $good md5 checksums are ok"

popd

########################################################################
header "Tests finished"

echo " "
echo "Ok!"
exit 0
