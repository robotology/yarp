/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


// WARNING This is a temporary hack until yarp::os::Thread::join method is public

#ifndef ROBOTINTERFACE_THREAD_H
#define ROBOTINTERFACE_THREAD_H

#include <yarp/os/Thread.h>

namespace RobotInterface
{

class Thread: public yarp::os::Thread
{
public:
    virtual void abort() {}
};

} // RobotInterface

#endif // ROBOTINTERFACE_THREAD_H
