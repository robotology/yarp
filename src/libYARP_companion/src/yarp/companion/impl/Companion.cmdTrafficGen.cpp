/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/PeriodicThread.h>
#include <limits>
#include <cstring>

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::Property;
using namespace std;

class datasender : public yarp::os::PeriodicThread
{
private:
    double m_data_size_MB;
    char* data_buff =nullptr;
    std::string m_portname;
    yarp::os::Port outport;
    yarp::os::Bottle pp;

public:
    datasender(double period, string portname, double data_size_MB) :
            PeriodicThread (period),
            m_data_size_MB(data_size_MB),
            m_portname(std::move(portname))
    {
    }

    bool threadInit() override
    {
        size_t data_size_B= (size_t)(m_data_size_MB*1000000.0);
        data_buff = new char[data_size_B +1];
        memset(data_buff,'0', data_size_B);
        data_buff[data_size_B]=0;

        if (!outport.open(m_portname))
        {
            yCError(COMPANION, "Failed to open port: %s", m_portname.c_str());
            return false;
        }

        pp.addString(std::string(data_buff).c_str());
        return true;
    }

    void run() override
    {
        if (outport.getOutputCount() > 0) {
            outport.write(pp);
        }
    }

    void threadRelease() override
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
        yCInfo(COMPANION, "This is yarp trafficgen. Syntax/available options:");
        yCInfo(COMPANION, "  yarp trafficgen /port --bandwidth [Mb/s]");
        yCInfo(COMPANION, "  yarp trafficgen /port --bandwidth [Mb/s] --duration [s]");
        yCInfo(COMPANION, "  yarp trafficgen /port --period [s] --size [MB]");
        yCInfo(COMPANION, "  yarp trafficgen /port --period [s] --size [MB] --duration [s]");
        return 1;
    }

    double period;
    double size_MB;
    double duration = options.check("duration", yarp::os::Value(std::numeric_limits<double>::infinity()), "duration (s)").asFloat32();

    if (options.check("bandwidth") && !options.check("size") && !options.check("period"))
    {
        double bandwidth_Mbs = options.check("bandwidth", yarp::os::Value(1), "bandwidth (Mb/s)").asFloat32();
        period = 1.0;
        size_MB = bandwidth_Mbs/8;
    }
    else if (!options.check("bandwidth") && options.check("size") && options.check("period"))
    {
        period = options.check("period", yarp::os::Value(1), "period (s)").asFloat32();
        size_MB = options.check("size", yarp::os::Value(1), "size (MB)").asFloat32();
    }
    else if (!options.check("bandwidth") && !options.check("size") && !options.check("period"))
    {
        period = 1.0;
        size_MB = 1.0;
    }
    else
    {
        yCError(COMPANION, "Invalid combination of options. Please check the available options with: yarp trafficgen --help");
        return 0;
    }

    string portname = argv[0];

    yCInfo (COMPANION, "Starting trafficgen with the following options: period:%.3f(s), size:%.3f(MB), duration:%.3f(s), bandwidth:%.3f(Mb/s) ", period, size_MB, duration, size_MB / period *8);

    datasender pt(period, portname, size_MB);
    double start_time = yarp::os::Time::now();
    if (pt.start() == false)
    {
        yCError(COMPANION, "Failed to start the main thread");
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
