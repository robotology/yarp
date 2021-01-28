/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <yarp/os/Port.h>
#include <yarp/os/Thread.h>

#include <string>


class CollatzClient : public yarp::os::Thread
{
protected:
    yarp::os::Port port;
    std::string portName;
    unsigned int replyField;

    void verifyItem(const unsigned int num, const unsigned int thres);

public:
    CollatzClient(const std::string &_portName);

    virtual bool threadInit();
    virtual void run();
    virtual void threadRelease();
};


#endif
