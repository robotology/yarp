/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <yarp/os/Bottle.h>
#include <yarp/os/Port.h>
#include <yarp/os/Thread.h>

#include <string>
#include <list>


class CollatzServer : public yarp::os::Thread
{
protected:
    yarp::os::Port port;
    std::string portName;
    std::list<unsigned int> table;
    unsigned int curNum;

    void tickItem(const unsigned int num);
    void generateItem(unsigned int &num, unsigned int &thres);

public:
    CollatzServer(const std::string &_portName);

    virtual bool threadInit();
    virtual void run();
    virtual void threadRelease();
};


#endif
