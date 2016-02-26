/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Network.h>
using namespace yarp::os;

#if YARP_USE_PERSISTENT_NAMESERVER
#  include <yarp/serversql/yarpserversql.h>
#endif

int main(int argc, char *argv[]) {
#if YARP_USE_PERSISTENT_NAMESERVER
    // intercept "yarp server" if needed
    if (argc>=2) {
        if (ConstString(argv[1])=="server") {
            return yarpserver3_main(argc,argv);
        }
    }
#endif

    // call the yarp standard companion
    Network yarp;
    return Network::main(argc,argv);
}

