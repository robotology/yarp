/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/PeriodicThread.h>

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::BufferedPort;
using yarp::os::NetworkBase;
using yarp::os::Property;
using namespace std;

class datasender : public yarp::os::PeriodicThread
{
private:
    size_t data_size = 1;
    char* data_buff =nullptr;
    std::string portname = "/trafficGen";
    yarp::os::BufferedPort<yarp::os::Bottle> outport;

public:
    datasender(double period) : PeriodicThread (period)
    {
    }

    bool threadInit()
    {
        data_buff = new char[data_size*1000000];
 
        if (!outport.open(portname))
        {
            yCError(COMPANION, "Failed to open port: %s", portname);
            return false;
        }

        Bottle& pp = outport.prepare();
        pp.addString(data_buff);
    }

    void run() override
    {
        if (outport.getOutputCount()>0)
            outport.write();
    }

    void threadRelease()
    {
        outport.interrupt();
        outport.close();
    }
};

int Companion::cmdTrafficGen(int argc, char *argv[])
{
    Property options;
    options.fromCommand(argc, argv, false);
    if (argc==0 || options.check("help"))
    {
        yCInfo(COMPANION, "This is yarp trafficGen. Syntax:");
        yCInfo(COMPANION, "  yarp trafficGen /port --period [s] --size [MB] --duration [s]");
        yCInfo(COMPANION, "  yarp trafficGen /port --bandwidth [Mb/s] --duration [s]");
        return 1;
    }

    double period = 1;
    string portname = "/trafficGen";
    size_t size = 1;
    double duration = 10;

    datasender pt(period);
    double start_time = yarp::os::Time::now();
    if (pt.start() == false)
    {
        yCInfo(COMPANION, "Failed to start the main thread");
        return 0;
    }

    do
    {
        yarp::os::Time::delay(0.1);
    }   while (yarp::os::Time::now() - start_time < duration);
    pt.stop();
    yarp::os::Time::delay(1);

    return 0;
}
