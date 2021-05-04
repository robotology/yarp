#!/bin/bash

# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

echo "Running yarpdev"
yarpdev --device controlboard --subdevice fakeMotionControl --GENERAL::Joints 4 &

sleep 1


echo "Now running stressrpc"

./stressrpc --id 0 --time 10 --prot udp &
./stressrpc --id 1 --time 10 --prot udp &
./stressrpc --id 2 --time 10 --prot udp &
./stressrpc --id 3 --time 10 --prot udp &
