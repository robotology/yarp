/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Network.h>
#include <yarp/serversql/yarpserversql.h>


using namespace yarp::os;


int main(int argc, char *argv[]) {
    // intercept "yarp server" if needed
    if (argc>=2) {
        if (ConstString(argv[1])=="server") {
            return yarpserver_main(argc,argv);
        }
    }

    // call the yarp standard companion
    Network yarp(yarp::os::YARP_CLOCK_SYSTEM);
    return Network::main(argc,argv);
}

