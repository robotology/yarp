#!/bin/bash

PROTOCOLS="tcp udp"
PAYLOAD="8192 16384 32768 65536 131072 262144 524288 1048576"
PL_VERBOSE=( 8k 16k 32k 64k 128k 256k 512k 1M )
NFRAMES=10

CLIENT_PORT=/profiling/client/end/port:i
SERVER_PORT=/profiling/server/default/port:o

#MACHINES=( icub-b1 )
MACHINES=( )

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
		k=0
		for payload in $PAYLOAD
		do
			for protocol in $PROTOCOLS
			do
				echo "Starting server"
				./port_latency --server --period $RATE --payload $payload &
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
				./port_latency --client --name end --nframes $NFRAMES &
				jobClient=$!
				echo $jobClient

				yarp wait $CLIENT_PORT

				if [ "$numMachines" -gt 1 ]; then
					lastMachine=${MACHINES[($numMachines)-1]}
					yarp connect /profiling/client/$lastMachine/port:o $CLIENT_PORT $protocol
					fistMachine=${MACHINES[0]}
					yarp connect $SERVER_PORT /profiling/client/$fistMachine/port:i $protocol
				else
					yarp connect $SERVER_PORT $CLIENT_PORT $protocol
				fi

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
				reportFile="rep-${PL_VERBOSE[$k]}-$protocol-$nodes.txt"
				mv timing.txt $reportFile
			done
			k=$(($k+1))
		done
		;;
esac
