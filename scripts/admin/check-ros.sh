#!/bin/bash

# Copyright: (C) 2013 iCub Facility
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

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

YARP_DIR=/root/yarp/bin
BASE=$PWD/check_ros_

########################################################################
header "Check name server is found"

${YARP_DIR}/yarp where || exit 1


########################################################################
header "Test name gets listed"

${YARP_DIR}/yarp read /test/msg@/test_node &
YPID=$!

wait_node_topic /test_node /test/msg

kill $YPID

echo "Topic should now be gone"
rostopic info /test_msg && exit 1 || echo "(this is correct)."

########################################################################
header "Test yarp write name gets listed with right type"

typ="test_write/pid$$"
topic="/test/msg/$typ"
yes | ${YARP_DIR}/yarp write $topic@/test_node --type $typ &
YPID=$!

wait_node_topic /test_node $topic

if [ ! "k`rostopic info $topic | grep 'Type:'`" = "kType: $typ" ]; then
    echo "Type problem:"
    rostopic info $topic
    kill $YPID
    echo "That is not right at all"
    exit 1
fi

kill $YPID

echo "Topic should now be gone"
rostopic info $topic && exit 1 || echo "(this is correct)."


########################################################################
header "Test yarp read name gets listed with right type"

typ="test_read/pid$$"
topic="/test/msg/$typ"
${YARP_DIR}/yarp read $topic@/test_node --type $typ &
YPID=$!

wait_node_topic /test_node $topic

if [ ! "k`rostopic info $topic | grep 'Type:'`" = "kType: $typ" ]; then
    echo "Type problem:"
    rostopic info $topic
    kill $YPID
    echo "That is not right at all"
    exit 1
fi

kill $YPID

echo "Topic should now be gone"
rostopic info $topic && exit 1 || echo "(this is correct)."


########################################################################
header "Test against rospy_tutorials/listener"

rm -f ${BASE}listener.log
touch ${BASE}listener.log
stdbuf --output=L rosrun rospy_tutorials listener > ${BASE}listener.log &

echo "Hello" | ${YARP_DIR}/yarp write /chatter@/ros/check/write --type std_msgs/String --wait-connect

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
stdbuf --output=L ${YARP_DIR}/yarp read /chatter@/ros/check/read > ${BASE}talker.log &

wait_file ${BASE}talker.log
result=`cat ${BASE}talker.log | head -n1 | sed "s/world .*/world/" | sed "s/[^a-z ]//g"`

for f in `rosnode list | grep "^/talker"`; do
    echo $f
    rosnode kill $f
done

killall ${YARP_DIR}/yarp

echo "Result is '$result'"
if [ ! "hello world" = "$result" ] ; then
    echo "That is not right."
    exit 1
fi

########################################################################
header "Test against rospy_tutorials/add_two_ints_server"

rm -f ${BASE}add_two_ints_server.log
touch ${BASE}add_two_ints_server.log

${YARP_DIR}/yarpidl_rosmsg --name /typ@/yarpros &
PIDL=$!
wait_node /yarpros /typ

rosrun rospy_tutorials add_two_ints_server &
wait_node /add_two_ints_server /add_two_ints
echo "10 20" | ${YARP_DIR}/yarp rpc /add_two_ints > ${BASE}add_two_ints_server.log

result=`cat ${BASE}add_two_ints_server.log | sed "s/.* //"`

for f in `rosnode list | grep "^/add_two_ints_server"`; do
    echo $f
    rosnode kill $f
done

kill $PIDL

echo "Result is '$result'"
if [ ! "30" = "$result" ] ; then
    echo "That is not right."
    exit 1
fi

########################################################################
header "Tests finished"

echo " "
echo "Ok!"