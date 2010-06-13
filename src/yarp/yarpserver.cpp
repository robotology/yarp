// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/Companion.h>

#include <yarp/os/Network.h>

using namespace yarp::os;
using namespace yarp::os::impl;

int main(int argc, char *argv[]) {
    Network yarp;

    // call the yarp standard companion name server
    argc--;
    argv++;
    int result = Companion::getInstance().cmdServer(argc,argv);
    return result;
}

