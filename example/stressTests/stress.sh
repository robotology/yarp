#!/bin/bash

# Copyright: (C) 2010 RobotCub Consortium
# Author: Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

echo "Running yarpdev"
yarpdev --device controlboard --subdevice test_motor &

sleep 1


echo "Now running stressrpc"

./stressrpc --id 0 --time 10 --prot udp &
./stressrpc --id 1 --time 10 --prot udp &
./stressrpc --id 2 --time 10 --prot udp &
./stressrpc --id 3 --time 10 --prot udp &


