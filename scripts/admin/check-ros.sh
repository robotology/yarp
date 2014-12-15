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

echo "Run some basic ROS tests, assuming a ros install"
echo "Also assumes that YARP has been configured for ROS"

BASE=$PWD/check_ros_

########################################################################
header "Start name servers if needed"

rostopic list || {
    echo "String roscore"
    roscore > /dev/null &
}

while ! rostopic list; do
    echo "Waiting for roscore"
    sleep 1
done

${YARP_BIN}/yarp where || {
    echo "Starting yarpserver"
    ${YARP_BIN}/yarpserver --ros --write > /dev/null &
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
YPID=$!

wait_node_topic /test_node /test/msg

kill $YPID
wait $YPID

echo "Topic should now be gone"
rostopic info /test_msg && exit 1 || echo "(this is correct)."

########################################################################
header "Test yarp write name gets listed with right type"

typ="test_write/pid$$"
topic="/test/msg/$typ"
yes | ${YARP_BIN}/yarp write $topic@/test_node --type $typ &
YPID=$!

wait_node_topic /test_node $topic

if [ ! "k`rostopic info $topic | grep 'Type:'`" = "kType: $typ" ]; then
    echo "Type problem:"
    rostopic info $topic
    kill $YPID
    wait $YPID
    echo "That is not right at all"
    exit 1
fi

${YARP_BIN}/yarp terminate $topic@/test_node
while ${YARP_BIN}/yarp exists $topic@/test_node ; do
    echo "Waiting for port to disappear"
    sleep 1
done

kill $YPID
wait $YPID

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
YPID=$!

wait_node_topic /test_node $topic

if [ ! "k`rostopic info $topic | grep 'Type:'`" = "kType: $typ" ]; then
    echo "Type problem:"
    rostopic info $topic
    kill $YPID
    wait $YPID
    echo "That is not right at all"
    exit 1
fi

kill $YPID
wait $YPID

echo "Topic should now be gone"
rostopic info $topic && exit 1 || echo "(this is correct)."


########################################################################
header "Test yarp read name gets listed with right type using twiddle"

typ="test_twiddle/pid$$"
topic="/test/msg/$typ"
${YARP_BIN}/yarp read $topic@/test_node~$typ &
YPID=$!

wait_node_topic /test_node $topic

if [ ! "k`rostopic info $topic | grep 'Type:'`" = "kType: $typ" ]; then
    echo "Type problem:"
    rostopic info $topic
    kill $YPID
    wait $YPID
    echo "That is not right at all"
    exit 1
fi

kill $YPID
wait $YPID

echo "Topic should now be gone"
rostopic info $topic && exit 1 || echo "(this is correct)."


########################################################################
header "Test against rospy_tutorials/listener"

rm -f ${BASE}listener.log
touch ${BASE}listener.log
stdbuf --output=L rosrun rospy_tutorials listener > ${BASE}listener.log &

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

########################################################################
header "Test against rospy_tutorials/talker"

rm -f ${BASE}talker.log
touch ${BASE}talker.log

rosrun rospy_tutorials talker &
stdbuf --output=L ${YARP_BIN}/yarp read /chatter@/ros/check/read > ${BASE}talker.log &
YPID=$!

wait_file ${BASE}talker.log
result=`cat ${BASE}talker.log | head -n1 | sed "s/world .*/world/" | sed "s/[^a-z ]//g"`

for f in `rosnode list | grep "^/talker"`; do
    echo $f
    rosnode kill $f
done

kill $YPID
wait $YPID

echo "Result is '$result'"
if [ ! "hello world" = "$result" ] ; then
    echo "That is not right."
    exit 1
fi

########################################################################
header "Test against rospy_tutorials/add_two_ints_server"

rm -f ${BASE}add_two_ints_server.log
touch ${BASE}add_two_ints_server.log

rm -f rospy_tutorials_AddTwoInts

${YARP_BIN}/yarpidl_rosmsg --name /typ@/yarpros --web false &
YPID=$!
wait_node /yarpros /typ

rosrun rospy_tutorials add_two_ints_server &
wait_node /add_two_ints_server /add_two_ints
echo "10 20" | ${YARP_BIN}/yarp rpc /add_two_ints > ${BASE}add_two_ints_server.log

result=`cat ${BASE}add_two_ints_server.log | sed "s/.* //"`

for f in `rosnode list | grep "^/add_two_ints_server"`; do
    echo $f
    rosnode kill $f
done

kill $YPID
wait $YPID

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
YPID=$!

wait_node_topic $node $topic

if [ ! "k`rostopic info $topic | grep 'Type:'`" = "kType: $typ" ]; then
    echo "Type problem:"
    rostopic info $topic
    kill $YPID
    echo "That is not right at all"
    exit 1
fi

rostopic echo $topic -n 1 > ${BASE}image.log
height=`cat ${BASE}image.log | grep "height:" | sed "s/.* //"`
width=`cat ${BASE}image.log | grep "width:" | sed "s/.* //"`
echo "width x height = $width x $height"

if [ ! "$width x $height" = "16 x 8" ] ; then
    echo "Size is not right"
    kill $YPID
    wait $YPID
    exit 1
else
    echo "Size is correct"
fi
kill $YPID
wait $YPID

########################################################################
header "Test ros images arrive"

node="/test/rimage/node/pid$$"
typ="sensor_msgs/Image"
topic="/test/rimage/$typ/pid$$"

rm -f ${BASE}rimage.log
touch ${BASE}rimage.log
stdbuf --output=L ${YARP_BIN}/yarp read $topic@$node > ${BASE}rimage.log &
YPID=$!
wait_node_topic $node $topic
rostopic pub -f ${BASE}image.log $topic sensor_msgs/Image -1

wait_file ${BASE}rimage.log

grep "\[mat\] \[rgb\]" ${BASE}rimage.log && echo "(got an image, good)" || {
    echo "did not get an image"
    kill $YPID
    wait $YPID
    exit 1
}
kill $YPID
wait $YPID

########################################################################
header "Tests finished"

echo " "
echo "Ok!"
exit 0
