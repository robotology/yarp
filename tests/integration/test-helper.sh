#!/bin/bash

########################################################################
# Minimal framework for bash integration tests
#
# functions:
#    require_name_server  - make sure a yarp name server is available
#    get_uri              - convert a port name to I.P.AD.DRESS:NUMBER
#    add_helper $1        - store a PID of something that should be 
#                           killed on emergency exit/abort
#    cleanup_helper       - make sure helper is gone, killing it if
#                           necessary
#
#  traps are added to cleanup any server started or lingering helper.
#


set -e
if [ ! -e CMakeCache.txt ] ; then
    echo "Please run from build directory"
    exit 1
fi

YARP_BIN="$PWD/bin"

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

function add_helper {
    HELPER_ID="$1"
}

function cleanup_helper() {
    if [ ! "k$HELPER_ID" = "k" ]; then
	kill $HELPER_ID || true
	wait $HELPER_ID || true
	HELPER_ID=""
    fi
}

function cleanup_all() {
    header "Shutting down"
    cleanup_helper
    if [ ! "k$SERVER_ID" = "k" ]; then
	kill $SERVER_ID || true
	wait $SERVER_ID || true
	SERVER_ID=""
    fi
}
trap cleanup_all EXIT

function require_name_server() {
    header "Start name server if needed and add cleanup traps"

    ${YARP_BIN}/yarp where || {
	echo "Starting yarpserver"
	${YARP_BIN}/yarpserver --write > /dev/null &
	SERVER_ID=$!
    }

    while ! ${YARP_BIN}/yarp detect --write; do
	echo "Waiting for yarpserver"
	sleep 1
    done

    echo "yarpserver is OK"
}
