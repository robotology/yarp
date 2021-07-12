#!/bin/bash

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

killall yarp yarpdev
killall -9 yarp yarpdev
sleep 2

yarpdev --device fakeFrameGrabber --framerate 0.5 --width 4 --height 4 --name /src &
yarp wait /src
sleep 2

echo "================================================="
echo "=="
echo "thread count for yarpdev with 0 outputs"
ps -eLf | grep "yarpdev" | wc
echo "=="
echo "================================================="

yarp read /dest1 mcast://src &
yarp wait /src /dest1
sleep 2

echo "================================================="
echo "=="
echo "thread count for yarpdev with 1 output"
ps -eLf | grep "yarpdev" | wc
echo "=="
echo "================================================="

yarp read /dest2 mcast://src &
yarp wait /src /dest2
sleep 2

echo "================================================="
echo "=="
echo "thread count for yarpdev with 2 outputs"
ps -eLf | grep "yarpdev" | wc
echo "=="
echo "================================================="

yarp read /dest3 mcast://src &
yarp wait /src /dest2
sleep 2

echo "================================================="
echo "=="
echo "thread count for yarpdev with 3 outputs"
ps -eLf | grep "yarpdev" | wc
echo "=="
echo "================================================="


killall yarp yarpdev
killall -9 yarp yarpdev
