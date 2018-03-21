#!/bin/bash

# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $SCRIPT_DIR/test-helper.sh
require_name_server

########################################################################
header "Check rpc replies get through on immediate shutdown"

for (( k=0; k<25; k++ )); do
    port_name=/rpc/$$/$k
    ${YARP_BIN}/yarp rpcserver --stop --echo $port_name &
    ${YARP_BIN}/yarp wait $port_name
    echo hello | ${YARP_BIN}/yarp rpc $port_name
    wait_port_gone $port_name
done

