#!/bin/bash

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

########################################################################
# Minimal framework for bash integration tests
#
# functions:
#    header $text         - print a header for a test
#    require_name_server  - make sure a yarp name server is available
#    require_ros_name_server - make sure a ros name server is available
#    get_uri              - convert a port name to I.P.AD.DRESS:NUMBER
#    add_helper $pid      - store a PID of something that should be
#                           killed on emergency exit/abort
#    cleanup_helper       - make sure helper is gone, killing it if
#                           necessary
#    cleanup_helper $sig  - send signal, then make sure helper is gone
#    wait_file $fname     - wait for a file to exist and have some
#                           content
#    wait_node_topic $n $t - wait for a node to pub/sub topic
#    wait_node_service $n $s - wait for a node to offer/use service
#    wait_topic_gone $t   - wait for a topic to disappear
#    wait_node $node $key - wait for key to disappear from node description
#
#  traps are added to cleanup any server started or lingering helper.
#

set -e
if [ ! -e CMakeCache.txt ] ; then
    echo "Please run from build directory"
    exit 1
fi

YARP_BIN="$PWD/bin"

# Print a header for the test
function header {
echo " "
echo "====================================================================="
echo "== $1"
echo " "
}

# Convert a port name to I.P.AD.DRESS:NUMBER
function get_uri {
    port=$1
    uri=$(${YARP_BIN}/yarp name query $port | sed "s/.* ip //" | sed "s/ port /:/" | sed "s/ .*//")
    echo $uri
}

# Track PIDs of some processes we'll be running, for cleanup purposes
SERVER_ID=""
HELPER_ID=""
HELPER2_ID=""
ROS_ID=""

function add_helper {
    if [[ "$HELPER_ID" = "" ]]; then
        HELPER_ID="$1"
    elif [[ "$HELPER2_ID" = "" ]]; then
        HELPER2_ID="$1"
    else
        echo "Only two helpers supported in test-helper.sh"
        exit 1
    fi
}

function cleanup_helper() {
    if [[ ! "$HELPER_ID" = "" ]]; then
        if [[ ! "$1" = "" ]]; then
            kill -s $1 $HELPER_ID || true
        else
            kill $HELPER_ID || true
        fi
        wait $HELPER_ID || true
        HELPER_ID=""
    fi
    if [[ ! "$HELPER2_ID" = "" ]]; then
        if [[ ! "$1" = "" ]]; then
            kill -s $1 $HELPER2_ID || true
        else
            kill $HELPER2_ID || true
        fi
        wait $HELPER2_ID || true
        HELPER2_ID=""
    fi
}

function cleanup_all() {
    cleanup_helper
    if [ ! "k$SERVER_ID" = "k" ]; then
        header "Shutting down yarp server"
        kill $SERVER_ID || true
        wait $SERVER_ID || true
        SERVER_ID=""
    fi
    if [ ! "k$ROS_ID" = "k" ]; then
        header "Shutting down ros server"
        kill $ROS_ID || true
        wait $ROS_ID || true
        ROS_ID=""
    fi
}
trap cleanup_all EXIT

function require_name_server() {
    header "Start yarp name server if needed"

    args=""
    if [[ ! "$ROS_ID" = "" ]]; then
        args="--ros"
    fi
    ${YARP_BIN}/yarp where || {
        echo "Starting yarpserver"
        ${YARP_BIN}/yarpserver --write $args > /dev/null &
        SERVER_ID=$!
    }

    while ! ${YARP_BIN}/yarp detect --write; do
        echo "Waiting for yarpserver"
        sleep 1
    done

    echo "yarpserver is OK"
}

function require_ros_name_server() {
    command -v roscore >/dev/null 2>&1 || { echo "roscore not installed." >&2; return 1; }
    header "Start ros name server if needed"
    rostopic list || {
        echo "String roscore"
        roscore > /dev/null &
        ROS_ID=$!
    }
    while ! rostopic list; do
        echo "Waiting for roscore"
        sleep 1
    done
    echo "roscore is OK"
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
    while [ "`rostopic info $topic | grep $node | wc -c`" = "0" ] ; do
        echo "waiting for $node on $topic"
        sleep 1
    done
}

function wait_node_service {
    node="$1"
    topic="$2"
    rosservice info $topic | grep $node
    while [ "`rosservice info $topic | grep $node | wc -c`" = "0" ] ; do
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

function wait_port_gone {
    port="$1"
    while ${YARP_BIN}/yarp exists $port ; do
        echo "waiting for $port to disappear"
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
