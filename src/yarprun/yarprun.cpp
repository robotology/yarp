// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Author Alessandro Scalzo alessandro@liralab.it
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


// The main body of yarprun is now part of the libYARP_OS library, in:
//   src/libYARP_OS/src/Run.cpp

#include <yarp/os/Run.h>

int main(int argc, char *argv[]) 
{
    yarp::os::Network yarp;
    return yarp::os::Run::main(argc,argv);
}
