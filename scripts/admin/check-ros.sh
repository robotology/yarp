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

echo "Run some basic ROS tests, assuming a ros install"
echo "Also assumes that YARP has been configured for ROS"

YARP_DIR=/root/yarp/bin
BASE=$PWD/check_ros_

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

echo " "
echo "Ok!"