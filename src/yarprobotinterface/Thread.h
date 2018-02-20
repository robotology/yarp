/*
 * Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */


// WARNING This is a temporary hack until yarp::os::Thread::join method is public

#ifndef YARP_YARPROBOTINTERFACE_THREAD_H
#define YARP_YARPROBOTINTERFACE_THREAD_H

#include <yarp/os/Thread.h>

namespace RobotInterface
{

class Thread: public yarp::os::Thread
{
public:
    virtual void abort() {}
};

} // RobotInterface

#endif // YARP_YARPROBOTINTERFACE_THREAD_H
