#!/bin/bash

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# This shell script will only work if "server_peek" is compiled

case "$1" in
    "reset")
    rm -f yarp.db
    ./server_peek
    echo "Reset yarp.db"
    ;;
    "dump")
    echo ".dump" | sqlite3 yarp.db
    ;;
    "demo")
    ./server_peek --add port=/root
    ./server_peek --with port=/root --add host=127.0.0.1 host=10.0.0.2
    ./server_peek --with port=/root --add carrier=tcp socket=10012
    ./server_peek --add port=/test
    ./server_peek --with port=/test --add host=127.0.0.1 host=10.0.0.3
    ./server_peek --with port=/test --add carrier=tcp socket=10044
    ./server_peek --add port=/doomed
    ./server_peek --with port=/doomed --add host=127.0.0.1 host=10.0.0.5
    ./server_peek --with port=/doomed --add carrier=mcast socket=10000
    echo "Listing ports"
    ./server_peek --query "port=*"
    echo "Checking host names for /root"
    ./server_peek --with port=/root --query "host=*"
    echo "Removing /doomed"
    ./server_peek --delete "port=/doomed"
    ./server_peek --prune
    echo "Listing ports"
    ./server_peek --query "port=*"
    echo "Changing /root to /pippo"
    ./server_peek --with port=/root --set =/pippo
    echo "Listing ports"
    ./server_peek --query "port=*"
    echo "Changing /pippo carrier to udp"
    ./server_peek --with port=/pippo --set carrier=udp
    echo "Showing /pippo carrier"
    ./server_peek --with port=/pippo --query carrier=*
    ;;
    "setup")
    ./ctrl.sh reset
    ./ctrl.sh demo
    ;;
esac
