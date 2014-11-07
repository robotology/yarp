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
#include <yarp/os/Log.h>

yarp::os::LogForwarder* yarp::os::LogForwarder::instance = NULL;
yarp::os::LogForwarderDestroyer yarp::os::LogForwarder::destroyer;
yarp::os::Semaphore *yarp::os::LogForwarder::sem = NULL;

yarp::os::LogForwarder* yarp::os::LogForwarder::getInstance()
{
    if (!instance)
    {
        instance = new LogForwarder;
        destroyer.SetSingleton(instance);
    }
    return instance;
};

void yarp::os::LogForwarder::forward (std::string message)
{
    sem->wait();
    if (outputPort)
    {
        Bottle& b = outputPort->prepare();
        b.clear();
        std::string port = "["; port+=logPortName; port+="]";
        b.addString(port);
        b.addString(message);
        outputPort->write(true);
        outputPort->waitForWrite();
    }
    sem->post();
}

yarp::os::LogForwarder::LogForwarder()
{
    yarp::os::NetworkBase::initMinimum();
    sem = new yarp::os::Semaphore(1);
    yAssert(sem);
    outputPort =0;
    outputPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
    char host_name [MAX_STRING_SIZE]; //unsafe
    yarp::os::gethostname(host_name,MAX_STRING_SIZE);
    char prog_name [MAX_STRING_SIZE]; //unsafe
    yarp::os::getprogname(prog_name,MAX_STRING_SIZE);
    int pid = yarp::os::getpid();
    sprintf(logPortName, "/log/%s/%s/%d",host_name,prog_name,pid);  //unsafe, better to use snprintf when available
    outputPort->open(logPortName);
    yarp::os::Network::connect(logPortName, "/yarplogger");
    //yarp::os::Network::connect(logPortName, "/test");
};

yarp::os::LogForwarder::~LogForwarder()
{
    sem->wait();
    if (outputPort)
    {
        Bottle& b = outputPort->prepare();
        b.clear();
        std::string port = "["; port+=logPortName; port+="]";
        b.addString(port);
        b.addString("[INFO] Execution terminated\n");
        outputPort->write(true);
        outputPort->waitForWrite();
        //outputPort->interrupt();
        outputPort->close();
        delete outputPort;
        outputPort=0;
    }
    sem->post();
    delete sem;
    sem = NULL;
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

