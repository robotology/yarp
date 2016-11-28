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

yarp::os::LogForwarder* yarp::os::LogForwarder::instance = YARP_NULLPTR;
yarp::os::Semaphore *yarp::os::LogForwarder::sem = YARP_NULLPTR;

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
    // I believe this guy, which is called by a yDebug() or similar, should be always called after
    // yarp::os::Network has already been initialized, therefore calling initMinimum here is not required.
    // It should not harm, but I prefer to avoid it if possible
//     yarp::os::NetworkBase::initMinimum();
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
        outputPort=0;
    }
    sem->post();
    delete sem;
    sem = YARP_NULLPTR;
//     yarp::os::NetworkBase::finiMinimum();
};

