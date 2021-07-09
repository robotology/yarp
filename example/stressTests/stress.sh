#!/bin/bash

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

echo "Running yarpdev"
yarpdev --device controlboard --subdevice fakeMotionControl --GENERAL::Joints 4 &

sleep 1


echo "Now running stressrpc"

./stressrpc --id 0 --time 10 --prot udp &
./stressrpc --id 1 --time 10 --prot udp &
./stressrpc --id 2 --time 10 --prot udp &
./stressrpc --id 3 --time 10 --prot udp &
