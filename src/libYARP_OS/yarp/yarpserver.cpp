// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/Logger.h>
#include <yarp/NameClient.h>
#include <yarp/Companion.h>

#include <yarp/os/Network.h>

int main(int argc, char *argv[]) {
    // call the yarp standard companion name server
    ACE::init();
    argc--;
    argv++;
    int result = yarp::Companion::getInstance().cmdServer(argc,argv);
    ACE::fini();
    return result;
}

