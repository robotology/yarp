/*
 * Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/impl/LogForwarder.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/Time.h>

YARP_OS_API yarp::os::LogForwarder* yarp::os::LogForwarder::instance = NULL;
YARP_OS_API yarp::os::LogForwarderDestroyer yarp::os::LogForwarder::destroyer;

yarp::os::LogForwarder* yarp::os::LogForwarder::getInstance()
{
    sem.wait();
    if (!instance)
    {
        instance = new LogForwarder;
        destroyer.SetSingleton(instance);
    }
    sem.post();
    return instance;
};

void yarp::os::LogForwarder::forward (std::string message)
{
    sem.wait();
    if (outputPort)
    {
        Bottle& b = outputPort->prepare();
        b.clear();
        std::string port = "["; port+=logPortName; port+="]";
        b.addString(port);
        b.addString(message);
        outputPort->write(true);
    }
    sem.post();
}

yarp::os::LogForwarder::LogForwarder()
{
    yarp::os::NetworkBase::initMinimum();
    outputPort =0;
    outputPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
    char host_name [255]; //unsafe
    yarp::os::gethostname(host_name);
    char prog_name [255]; //unsafe
    yarp::os::getprogname(prog_name);
    int pid = yarp::os::getpid();
    sprintf(logPortName, "/log/%s/%s/%d",host_name,prog_name,pid);  //unsafe, better to use snprintf when available
    outputPort->open(logPortName);
    yarp::os::Network::connect(logPortName, "/yarplogger");
    //yarp::os::Network::connect(logPortName, "/test");
};

yarp::os::LogForwarder::~LogForwarder()
{
    if (outputPort)
    {
        Bottle& b = outputPort->prepare();
        b.clear();
        std::string port = "["; port+=logPortName; port+="]";
        b.addString(port);
        b.addString("[INFO] Execution terminated");
        outputPort->write();
        outputPort->waitForWrite();
        //outputPort->interrupt();
        outputPort->close();
        delete outputPort;
        outputPort=0;
    }
    yarp::os::NetworkBase::finiMinimum();
};

yarp::os::LogForwarderDestroyer::LogForwarderDestroyer(LogForwarder *s)
{
    singleton = s;
}

yarp::os::LogForwarderDestroyer::~LogForwarderDestroyer()
{
    if (singleton)
    {
        delete singleton;
    }
}

void yarp::os::LogForwarderDestroyer::SetSingleton(LogForwarder *s)
{
    singleton = s;
}

