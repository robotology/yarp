/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

