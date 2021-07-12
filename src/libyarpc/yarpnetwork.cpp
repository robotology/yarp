/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include "yarp.h"
#include "yarpimpl.h"

static yarpNetworkPtr __yarp_network;

    /**
     * Create a network.  There should typically be one of
     * these in a program using YARP.  You need one of these in order
     * to create ports.
     */
YARP_DEFINE(yarpNetworkPtr) yarpNetworkCreate() {
    yarpNetworkPtr network = new yarpNetwork;
    __yarp_network = network;
    if (network!=NULL) {
        network->implementation = new Network();
        if (network->implementation==NULL) {
            delete network;
            network = NULL;
        }
    }
    return network;
}


YARP_DEFINE(yarpNetworkPtr) yarpNetworkGet() {
    return __yarp_network;
}


    /**
     * Destroy a network.
     */
YARP_DEFINE(void) yarpNetworkFree(yarpNetworkPtr network) {
    if (network!=NULL) {
        if (network->implementation!=NULL) {
            delete (Network*)(network->implementation);
            network->implementation = NULL;
        }
        delete network;
    }
}

    /**
     * Put a network in local mode (no YARP name server needed).
     */
YARP_DEFINE(int) yarpNetworkSetLocalMode(yarpNetworkPtr network,
                                         int isLocal) {
    YARP_NETWORK(network).setLocalMode(isLocal);
    return 0;
}


    /**
     *
     * Connect two ports with a specified carrier (connection type).
     * If the carrier is NULL, the connection will be of type tcp.
     *
     */
YARP_DEFINE(int) yarpNetworkConnect(yarpNetworkPtr network,
                                    const char *src,
                                    const char *dest,
                                    const char *carrier) {
    YARP_NETWORK(network).connect(src,dest,carrier);
    return 0;
}


    /**
     *
     * Disconnect two ports from each other.
     *
     */
YARP_DEFINE(int) yarpNetworkDisconnect(yarpNetworkPtr network,
                                    const char *src,
                                    const char *dest) {
    YARP_NETWORK(network).disconnect(src,dest);
    return 0;
}
