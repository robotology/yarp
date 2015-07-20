// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Author Alessandro Scalzo alessandro@liralab.it
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


// The main body of yarprun is now part of the libYARP_OS library, in:
//   src/libYARP_OS/src/Run.cpp

#include <yarp/os/Run.h>
#include <yarp/os/Network.h>
#include <stdio.h>


int main(int argc, char *argv[]) 
{
    yarp::os::Network yarp;
    int ret=yarp::os::Run::main(argc,argv);
    return (ret!=0?1:0);
}
