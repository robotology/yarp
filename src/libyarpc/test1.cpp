/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include "yarp.h"

int main(int argc, char *argv[]) {
    int result = 0;

    printf("This test creates a port and then immediately closes it.\n");
    printf("Make sure no other YARP programs are running.\n");
    printf("(or else remove the yarpNetworkSetLocalMode line)\n");

    yarpNetworkPtr network = yarpNetworkCreate();
    if (network==NULL) return -1;

    // assume no other YARP programs are running, including the name server
    result = yarpNetworkSetLocalMode(network,1);
    if (result<0) return -1;

    yarpContactPtr contact = yarpContactCreate();
    if (contact==NULL) return -1;
    result = yarpContactSetName(contact,"/test");
    if (result<0) return -1;

    yarpPortPtr port = yarpPortCreate(network);
    if (port==NULL) return -1;
    result = yarpPortOpenEx(port,contact);
    if (result<0) return -1;

    yarpContactFree(contact);
    contact = NULL;

    result = yarpPortClose(port);
    if (result<0) return -1;

    yarpPortFree(port);
    port = NULL;

    yarpNetworkFree(network);
    network = NULL;

    return 0;
}
