/*
 * Copyright (C) 2009 RobotCub Consortium
 * Author Alessandro Scalzo alessandro.scalzo@iit.it
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


// The main body of yarprun is now part of the libYARP_OS library, in:
//   src/libYARP_OS/src/Run.cpp

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

    int ret=yarp::os::Run::main(argc,argv);
    return (ret!=0?1:0);
}
