#!/bin/bash

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

export ACE_ROOT=/home/icub/Code/ACE_wrappers
export YARP_DIR=/home/icub/Code/yarp2
export YARP_ROOT=$YARP_DIR
export YARP_CONFIG_DIR=$YARP_DIR/conf
export ICUB_DIR=/home/icub/Code/iCub
export ICUB_ROOT=$ICUB_DIR
export ICUB_CONF=$ICUB_DIR/conf
export PATH=$PATH:$YARP_ROOT/bin:$ICUB_ROOT/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ACE_ROOT/lib
PROTOCOLS="tcp udp"
RATES="5 10 20 30 40 50 60 70 80 90 100 200 300 400 500 1000 2000"
NFRAMES=1000

CLIENT_PORT=/profiling/client/end/port:i
SERVER_PORT=/profiling/server/default/port:o

for rate in $RATES
  do
  for protocol in $PROTOCOLS
  do
    echo "Starting server"
    ../port_latency --server --period $rate &
    jobServer=$!

    yarp wait $SERVER_PORT
    echo "Ok server exists"
    echo "Starting client"
    ../port_latency --client --name end --nframes $NFRAMES &
    jobClient=$!
    echo $jobClient

    yarp wait $CLIENT_PORT
    yarp connect $SERVER_PORT $CLIENT_PORT $protocol

    echo "Now waiting for test"
    wait $jobClient

    echo "Now killing server"
    kill $jobServer

    echo "Moving report file"
    reportFile="rep-$rate-$protocol.txt"
    mv timing.txt $reportFile
  done
done
