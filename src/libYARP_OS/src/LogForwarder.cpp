/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/LogForwarder.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>

yarp::os::LogForwarder* yarp::os::LogForwarder::instance = nullptr;
yarp::os::Semaphore *yarp::os::LogForwarder::sem = nullptr;

yarp::os::LogForwarder* yarp::os::LogForwarder::getInstance()
{
    if (!instance)
    {
        instance = new LogForwarder;
    }
    return instance;
};

void yarp::os::LogForwarder::clearInstance()
{
    if (instance)
    {
        delete instance;
    };
};

void yarp::os::LogForwarder::forward (const std::string& message)
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
    // I believe this guy, which is called by a yDebug() or similar, should be always called after
    // yarp::os::Network has already been initialized, therefore calling initMinimum here is not required.
    // It should not harm, but I prefer to avoid it if possible
//     yarp::os::NetworkBase::initMinimum();
    sem = new yarp::os::Semaphore(1);
    yAssert(sem);
    outputPort =nullptr;
    outputPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
    char host_name [MAX_STRING_SIZE]; //unsafe
    yarp::os::gethostname(host_name, MAX_STRING_SIZE);
    char prog_name [MAX_STRING_SIZE]; //unsafe
    yarp::os::getprogname(prog_name, MAX_STRING_SIZE);
    int pid = yarp::os::getpid();
    sprintf(logPortName, "/log/%s/%s/%d", host_name, prog_name, pid);  //unsafe, better to use snprintf when available
    if (outputPort->open(logPortName) == false)
    {
        printf("LogForwarder error while opening port %s\n", logPortName);
    }
    if (yarp::os::Network::connect(logPortName, "/yarplogger") == false)
    {
        printf("LogForwarder error while connecting port %s\n", logPortName);
    }
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
        outputPort=nullptr;
    }
    sem->post();
    delete sem;
    sem = nullptr;
//     yarp::os::NetworkBase::finiMinimum();
};
