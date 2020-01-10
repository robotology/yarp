/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// WARNING This is a temporary hack until yarp::os::Thread::join method is public

#ifndef YARP_ROBOTINTERFACE_THREAD_H
#define YARP_ROBOTINTERFACE_THREAD_H

#include <yarp/os/Thread.h>

#include <yarp/robotinterface/api.h>

namespace yarp {
namespace robotinterface {

class YARP_robotinterface_API Thread: public yarp::os::Thread
{
public:
    virtual void abort() {}
};

} // namespace robotinterface
} // namespace yarp

#endif // YARP_ROBOTINTERFACE_THREAD_H
