#!/bin/bash

echo "Running yarpdev"
yarpdev --device controlboard --subdevice test_motor &

sleep 1

echo "Now running stressrpc"

./stressrpc --id 0 --prot udp &
./stressrpc --id 1 --prot udp &
./stressrpc --id 2 --prot udp &
./stressrpc --id 3 --prot udp &

sleep 10

echo "Now killing stressrpc"
killall stressrpc

echo "Now killing yarpdev"
killall yarpdev



