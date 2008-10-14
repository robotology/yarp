// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/Companion.h>

#include <yarp/os/Network.h>

int main(int argc, char *argv[]) {
    // call the yarp standard companion
    ACE::init();
    int result = yarp::os::Network::main(argc,argv);
    ACE::fini();
    return result;
}

