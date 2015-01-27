#!/bin/bash

set -e
if [ ! -e CMakeCache.txt ] ; then
    echo "Please run from build directory"
    exit 1
fi

YARP_BIN="$PWD/bin"

########################################################################
# Some utilities and cleanup code

# Print a header for the test
function header {
echo " "
echo "====================================================================="
echo "== $1"
echo " "
}

# Convert a port name to I.P.AD.DRESS:NUMBER
function get_uri {
    port=$1
    uri=$(${YARP_BIN}/yarp name query $port | sed "s/.* ip //" | sed "s/ port /:/" | sed "s/ .*//")
    echo $uri
}

# Track PIDs of some processes we'll be running, for cleanup purposes
SERVER_ID=""
HELPER_ID=""

cleanup_helper() {
    if [ ! "k$HELPER_ID" = "k" ]; then
	kill $HELPER_ID || true
	wait $HELPER_ID || true
	HELPER_ID=""
    fi
}

cleanup_all() {
    cleanup_helper
    if [ ! "k$SERVER_ID" = "k" ]; then
	kill $SERVER_ID || true
	wait $SERVER_ID || true
	SERVER_ID=""
    fi
}
trap cleanup_all EXIT

########################################################################
header "Start name server if needed and add cleanup traps"

${YARP_BIN}/yarp where || {
    echo "Starting yarpserver"
    ${YARP_BIN}/yarpserver --write > /dev/null &
    SID=$!
}

while ! ${YARP_BIN}/yarp detect --write; do
    echo "Waiting for yarpserver"
    sleep 1
done

echo "yarpserver is OK"

########################################################################
header "Check that curl request succeeds"

port_name=/read/$$
${YARP_BIN}/yarp read $port_name &
HELPER_ID=$!
${YARP_BIN}/yarp wait $port_name

uri=$(get_uri $port_name)

which curl
curl $uri || {
    echo "ERROR: *** HTTP connection to name server failed ***"
    exit 1
}

echo "Curl request worked fine!"

header "Shutting down"
