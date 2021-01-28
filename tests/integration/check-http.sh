#!/bin/bash

# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $SCRIPT_DIR/test-helper.sh
require_name_server

########################################################################
header "Check that curl request succeeds"

port_name=/read/$$
${YARP_BIN}/yarp read $port_name &
add_helper $!
${YARP_BIN}/yarp wait $port_name

uri=$(get_uri $port_name)

which curl
curl $uri || {
    echo "ERROR: *** HTTP connection to name server failed ***"
    exit 1
}

echo "Curl request worked fine!"
