#!/bin/bash

./stressrpc --id 0 --prot udp &
./stressrpc --id 1 --prot udp &
./stressrpc --id 2 --prot udp &
./stressrpc --id 3 --prot udp &

sleep 10

killall stressrpc


