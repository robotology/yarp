/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// The main body of yarprun is now part of the libYARP_run library, in:
//   src/libYARP_run/src/Run.cpp

#include <yarp/os/Run.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <cstdio>


int main(int argc, char *argv[])
{
    yarp::os::Network yarp;

    if (!yarp.checkNetwork())
    {
        yError("Sorry YARP network does not seem to be available, is the yarp server available?\n");
        return 1;
    }

    return yarp::run::Run::main(argc,argv);
}
