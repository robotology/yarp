#!/bin/bash

./stressrpc --id 0 --time 10 --prot udp &
./stressrpc --id 1 --time 10 --prot udp &
./stressrpc --id 2 --time 10 --prot udp &
./stressrpc --id 3 --time 10 --prot udp &


