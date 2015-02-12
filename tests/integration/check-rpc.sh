#!/bin/bash

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $SCRIPT_DIR/test-helper.sh
require_name_server

########################################################################
header "Check rpc replies get through on immediate shutdown"

for (( k=0; k<25; k++ )); do
    port_name=/rpc/$$/$k
    ${YARP_BIN}/yarp rpcserver --stop --echo $port_name &
    add_helper $!
    ${YARP_BIN}/yarp wait $port_name
    echo hello | ${YARP_BIN}/yarp rpc $port_name
    cleanup_helper
done

