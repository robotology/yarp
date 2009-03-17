#!/bin/bash

echo "Running yarpdev"
yarpdev --device controlboard --subdevice fakebot &

sleep 1

echo "Now running stressrpc"
./stressrpc --id 0 --prot udp &

sleep 10

echo "Now killing stressrpc"
killall stressrpc

echo "Now killing yarpdev"
killall yarpdev

