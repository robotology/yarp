// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/Companion.h>

#include <yarp/os/Network.h>

using namespace yarp::os;
using namespace yarp::os::impl;

int main(int argc, char *argv[]) {
    // call the yarp standard companion name server
    ACE::init();
    argc--;
    argv++;
    int result = Companion::getInstance().cmdServer(argc,argv);
    ACE::fini();
    return result;
}

