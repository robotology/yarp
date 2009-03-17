#!/bin/bash

./stressrpc --id 0 --prot udp &

sleep 10

killall stressrpc


