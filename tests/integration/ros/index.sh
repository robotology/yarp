#!/bin/bash

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# Call without any arguments to get a list of available tests
# Call with the name of a test to run that test
# Call with the special name "all" to run all tests

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $SCRIPT_DIR/../test-helper.sh

active_test="$1"
available_tests=""

set -e

function is_test {
    if [[ ! "$available_tests" =~ " $1 " ]]; then
        available_tests="$available_tests $1 "
    fi
    test "k$active_test" = "k$1" -o "k$active_test" = "kall"
}


if [[ ! "$active_test" = "" ]]; then
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
fi

########################################################################
if [[ ! "$active_test" = "" ]]; then
    if [[ ! "$active_test" = "all" ]]; then
        require_ros_name_server || exit 1
        require_name_server
    fi
fi

########################################################################
if is_test name_registration; then
    header "Test name gets listed"

    ${YARP_BIN}/yarp read /test/msg@/test_node &
    add_helper $!

    wait_node_topic /test_node /test/msg

    cleanup_helper

    echo "Topic should now be gone"
    rostopic info /test_msg && exit 1 || echo "(this is correct)."
fi


########################################################################
if is_test bag_record; then
    header "Test bag file record and playback"

    root="/test/bag/pid$$"
    rm -rf ${BASE}check_bag
    mkdir -p ${BASE}check_bag
    pushd ${BASE}check_bag
    cmake $YARP_SRC/tests/integration/ros/ -DYARP_DIR=$YARP_DIR
    make

    rosrun rosbag record -a -O log.bag &
    add_helper $!
    sleep 5
    ./test_topic $root wait

    echo "Stopping rosbag process"
    cleanup_helper SIGINT
    test log.bag

    echo "Topic should now be gone"
    rostopic info $root/str && exit 1 || echo "(this is correct)."

    $YARP_BIN/yarp read $root/str@$root/reader > replay.txt &
    add_helper $!
    rosbag play log.bag --topics $root/str
    cleanup_helper

    grep "hello world" replay.txt || {
        echo "Failed to playback text"
        exit 1
    }

    rostopic echo $root/img -n 1 > replay_image.txt &
    add_helper $!
    rosbag play log.bag --topics $root/img
    cleanup_helper

    grep "is_bigendian" replay_image.txt || {
        echo "Failed to playback image"
        exit 1
    }

    rostopic echo $root/disp -n 1 > replay_disp.txt &
    add_helper $!
    rosbag play log.bag --topics $root/disp
    cleanup_helper

    grep "min_disparity" replay_disp.txt || {
        echo "Failed to playback disparity"
        exit 1
    }

    rostopic echo $root/cloud -n 1 > replay_cloud.txt &
    add_helper $!
    rosbag play log.bag --topics $root/cloud
    cleanup_helper

    grep "is_dense" replay_cloud.txt || {
        echo "Failed to playback pointcloud"
        exit 1
    }

    popd
fi


########################################################################
if is_test type_registration_write; then
    header "Test yarp write name gets listed with right type"

    typ="test_write/pid$$"
    topic="/test/msg/$typ"

    add_helper $( { { yes 0<&4 & echo $! >&3 ; } 4<&0 | ${YARP_BIN}/yarp write $topic@/test_node --type $typ >/dev/null & } 3>&1 | head -1 )

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
fi


########################################################################
if is_test type_registration_read; then
    header "Test yarp read name gets listed with right type"

    typ="test_read/pid$$"
    topic="/test/msg/$typ"
    ${YARP_BIN}/yarp read $topic@/test_node --type $typ &
    add_helper $!

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
fi


########################################################################
if is_test type_registration_twiddle; then
    header "Test yarp read name gets listed with right type using twiddle"

    typ="test_twiddle/pid$$"
    topic="/test/msg/$typ"
    ${YARP_BIN}/yarp read $topic@/test_node~$typ &
    add_helper $!

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
fi


########################################################################
if is_test against_tutorial_listener; then
    header "Test against rospy_tutorials/listener"

    rm -f ${BASE}listener.log
    touch ${BASE}listener.log
    stdbuf --output=L rosrun rospy_tutorials listener > ${BASE}listener.log &
    add_helper $!

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
fi


########################################################################
if is_test against_tutorial_talker; then
    header "Test against rospy_tutorials/talker"

    rm -f ${BASE}talker.log
    touch ${BASE}talker.log

    rosrun rospy_tutorials talker &
    stdbuf --output=L ${YARP_BIN}/yarp read /chatter@/ros/check/read > ${BASE}talker.log &
    add_helper $!

    wait_file ${BASE}talker.log
    result=`cat ${BASE}talker.log | grep -v "yarp:" | head -n1 | sed "s/world .*/world/" | sed "s/[^a-z ]//g"`

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
fi


########################################################################
if is_test against_tutorial_add_two_ints_server; then
    header "Test against rospy_tutorials/add_two_ints_server"

    rm -f ${BASE}add_two_ints_server.log
    touch ${BASE}add_two_ints_server.log

    rm -f rospy_tutorials_AddTwoInts

    ${YARP_BIN}/yarpidl_rosmsg --name /typ@/yarpros --web false &
    add_helper $!
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
fi


########################################################################
if is_test images_yarp_to_ros; then
    header "Test yarp images arrive"

    node="/test/image/node/pid$$"
    typ="sensor_msgs/Image"
    topic="/test/image/$typ/pid$$"
    echo ${YARP_BIN}/yarpdev --device fakeFrameGrabber --name $topic@$node --width 16 --height 8
    ${YARP_BIN}/yarpdev --device fakeFrameGrabber --name $topic@$node --width 16 --height 8 &
    add_helper $!

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
fi

########################################################################
if is_test images_ros_to_yarp; then
    header "Test ros images arrive"

    node="/test/rimage/node/pid$$"
    typ="sensor_msgs/Image"
    topic="/test/rimage/$typ/pid$$"

    rm -f ${BASE}rimage.log
    touch ${BASE}rimage.log
    stdbuf --output=L ${YARP_BIN}/yarp read $topic@$node > ${BASE}rimage.log &
    add_helper $!
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
fi


########################################################################
if is_test md5_all; then
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
fi


########################################################################
function begin_examples {
    dir=${BASE}check_example
    rm -rf $dir
    mkdir -p $dir
    pushd $dir
    cmake $YARP_SRC/example/ros/ -DYARP_DIR=$YARP_DIR
    make "$@"
    cp -R $YARP_SRC/example/ros/package package
    pushd package
    catkin_make
    popd
}

function end_examples {
    popd
}

function wait_for_log {
    want_lines="$1"
    max_wait="$2"
    at=0
    while true; do
        ct=`cat log.txt | wc -l`
        echo "waiting for at least $want_lines messages, have $ct"
        if [ $ct -gt $want_lines ]; then
            break
        fi
        if [ $at -gt $max_wait ]; then
            # approximate time, not exact
            echo "did not get messages"
            exit 1
        fi
        sleep 1
        let at=at+1
    done
}

########################################################################
if is_test cpp_talker_listener_v1; then
    header "test cpp example listener_v1 to talker"

    root="/test/example1/pid$$"
    begin_examples listener_v1 talker

    export YARP_RENAME_chatter__yarp_talker="$root@$root/talker"
    $PWD/talker &
    add_helper $!

    export YARP_RENAME_chatter__yarp_listener="$root@$root/listener"
    stdbuf --output=L $PWD/listener_v1 > log.txt &
    add_helper $!

    wait_for_log 3 30

    cleanup_helper
    end_examples
fi


########################################################################
if is_test cpp_talker_listener_v2; then
    header "test cpp example listener_v2 to talker"

    root="/test/example2/pid$$"
    begin_examples listener_v2 talker

    export YARP_RENAME_chatter__yarp_talker="$root@$root/talker"
    $PWD/talker &
    add_helper $!

    export YARP_RENAME_chatter__yarp_listener="$root@$root/listener"
    export YARP_RENAMEchatter="$root@$root/listener"
    stdbuf --output=L $PWD/listener_v2 > log.txt &
    add_helper $!

    wait_for_log 3 30

    cleanup_helper
    end_examples
fi

for client in v1 v1b v2; do
    for server in v1 v1b; do
        if is_test cpp_add_int_client_${client}_server_${server}; then
            header "test cpp example add_int_client_${client}/add_int_server_${server}"
            root="/test/example_add_int_${client}_${server}/pid$$"
            begin_examples add_int_server_${server} add_int_client_${client}
            source package/devel/setup.bash
            rossrv show yarp_test/AddTwoInts

            ${YARP_BIN}/yarpidl_rosmsg --name /typ@/yarpros --web false --verbose 1 &
            add_helper $!
            wait_node /yarpros /typ

            export YARP_RENAME_add_two_ints__yarp_add_int_server=/add_two_ints@$root/server
            export YARP_RENAME_add_two_ints__yarp_add_int_client=/add_two_ints@$root/client
            export YARP_RENAMEadd_two_ints="/add_two_ints@$root/server"
            $PWD/add_int_server_${server} &
            add_helper $!
            wait_node_service $root/server /add_two_ints

            export YARP_RENAMEadd_two_ints="/add_two_ints@$root/client"
            result=`$PWD/add_int_client_${client} 40 2 | sed "s/.* //"`
            echo "Result is $result"
            if [[ ! "$result" = "42" ]]; then
                echo "That is not right"
                exit 1
            fi

            cleanup_helper
            end_examples
        fi
    done
done

########################################################################
if is_test empty_list; then
    header "Test sending an empty list"

    root="/test/empty_list/pid$$"
    rm -f ${BASE}check_empty_list.txt

    cp $YARP_SRC/extern/ros/common_msgs/sensor_msgs/msg/JointState.msg sensor_msgs_JointState

    ${YARP_BIN}/yarpidl_rosmsg --name /typ@/yarpros --web false &
    add_helper $!
    wait_node /yarpros /typ

    rostopic echo $root -n 1 > ${BASE}check_empty_list.txt &
    add_helper $!

    echo '(6 (0 0) "0") (test) (1.0) (1.0) ()' | $YARP_BIN/yarp write $root@$root/node --type sensor_msgs/JointState --wait-connect

    wait_file ${BASE}check_empty_list.txt
    # getting past this line means receipt of a message

    cleanup_helper
fi


########################################################################
## We're done!
########################################################################

if [[ ! "$active_test" = "" ]]; then
    header "Test(s) finished"
    echo " "
    echo "Ok!"
    exit 0
fi

echo $available_tests
