#!/bin/bash

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

echo "Running yarpdev"
yarpdev --device controlboard --subdevice fakeMotionControl --GENERAL::Joints 4 &

sleep 1

echo "Now running stressrpc"
./stressrpc --id 0 --prot udp &

sleep 10

echo "Now killing stressrpc"
killall stressrpc

echo "Now killing yarpdev"
killall yarpdev
