// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdio.h>

#include "yarp.h"

int main(int argc, char *argv[]) {
    int result = 0;

    yarpNetworkPtr network = yarpNetworkCreate();
    if (network==NULL) return -1;

    yarpContactPtr contact = yarpContactCreate();
    if (contact==NULL) return -1;
    result = yarpContactSetName(contact,"/test");
    if (result<0) return -1;

    yarpPortPtr port = yarpPortCreate(network);
    if (port==NULL) return -1;
    result = yarpPortOpen(port,contact);
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


