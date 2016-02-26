/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Network.h>

using namespace yarp::os;

int main(int argc, char *argv[]) {
    Network yarp;
    return Network::runNameServer(argc,argv);
}

