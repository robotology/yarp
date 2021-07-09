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
NFRAMES=2000

CLIENT_PORT=/profiling/client/end/port:i
SERVER_PORT=/profiling/server/default/port:o

MACHINES=( pc104 )

case $1 in
    killall)

        for machine in ${MACHINES[@]}
        do
            echo "Killing client on $machine"
            yarp run --on /$machine --kill profTag
            echo echo "done"
        done

        killall port_latency
        ;;
    *)
        for rate in $RATES
        do
            for protocol in $PROTOCOLS
            do
                echo "Starting server"
                ../port_latency --server --period $rate &
                jobServer=$!

                yarp wait $SERVER_PORT
                echo "Ok server exists"

                for machine in ${MACHINES[@]}
                do
                    echo "Starting client on $machine"
                    cmd="$YARP_ROOT/example/profiling/port_latency --client --name $machine --nframes -1"
                    yarp run --on /$machine --cmd "$cmd" --as profTag
                    yarp wait /profiling/client/$machine/port:i
                    yarp wait /profiling/client/$machine/port:o
                    echo echo "done"
                done

                numMachines=${#MACHINES[@]}
                for (( i = 1 ; i < ($numMachines) ; i++ ))
                do
                    prev=${MACHINES[$(i-1)]}
                    machine=${MACHINES[$i]}
                    echo "Connecting $prev to $machine"
                    yarp connect /profiling/client/$prev/port:o /profiling/client/$machine/port:i $protocol
                    echo "done"
                done

                echo "Starting client on local machine"
                ../port_latency --client --name end --nframes $NFRAMES &
                jobClient=$!
                echo $jobClient

                yarp wait $CLIENT_PORT

                lastMachine=${MACHINES[($numMachines)-1]}
                yarp connect /profiling/client/$lastMachine/port:o $CLIENT_PORT
                fistMachine=${MACHINES[0]}
                yarp connect $SERVER_PORT /profiling/client/$fistMachine/port:i

                echo "Now waiting for test"
                wait $jobClient

                echo "Now killing server"
                kill $jobServer

                for machine in ${MACHINES[@]}
                do
                    echo "Killing client on $machine"
                    yarp run --on /$machine --kill profTag
                    echo echo "done"
                done

                echo "Moving report file"
                nodes=$(($numMachines+1))
                reportFile="rep-$rate-$protocol-$nodes.txt"
                mv timing.txt $reportFile
            done
        done
        ;;
esac
