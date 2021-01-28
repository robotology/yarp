/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Network.h>
#include <yarp/companion/yarpcompanion.h>
#include <yarp/serversql/yarpserversql.h>
#include <yarp/run/Run.h>


using namespace yarp::os;


int main(int argc, char *argv[]) {

    // intercept "yarp server" if needed
    if (argc>=2) {
        if (std::string(argv[1])=="server") {
            return yarpserver_main(argc,argv);
        }
    }

    Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    // intercept "yarp run" if needed
    if (argc>=2) {
        if (std::string(argv[1])=="run") {
            return yarp::run::Run::main(argc,argv);
        }
    }

    // call the yarp standard companion
    return yarp::companion::main(argc,argv);
}
