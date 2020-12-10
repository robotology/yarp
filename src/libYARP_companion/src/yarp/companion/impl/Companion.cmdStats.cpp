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
#include <yarp/os/Contactable.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/TypedReaderCallback.h>
#include <yarp/os/Value.h>

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::BufferedPort;
using yarp::os::Contactable;
using yarp::os::LogStream;
using yarp::os::NetworkBase;
using yarp::os::Property;
using yarp::os::Semaphore;
using yarp::os::Stamp;
using yarp::os::TypedReaderCallback;
using yarp::os::Value;

namespace {

class CompanionStatsInput :
        public TypedReaderCallback<Bottle>
{
public:
    Contactable *port{nullptr};
    std::mutex mutex;

private:
    Bottle value;
    size_t bottles_received=0;
    size_t bytes_received=0;
    Stamp stamp;

public:
    CompanionStatsInput() = default;
    void clearStats()
    {
        mutex.lock();
        bottles_received = 0;
        bytes_received = 0;
        mutex.unlock();
    }

    void getStats(size_t& b1, size_t& b2)
    {
        mutex.lock();
        b1= bytes_received;
        b2= bottles_received;
        mutex.unlock();
    }

    void init(Contactable& port)
    {
        this->port = &port;
    }

    using yarp::os::TypedReaderCallback<Bottle>::onRead;
    void onRead(Bottle& datum) override
    {
        mutex.lock();
        value = datum;
        bottles_received++;
        size_t  raw_bottle_bytes_c=0;
        const char*   raw_bottle_bytes_p = value.toBinary(&raw_bottle_bytes_c);
        bytes_received += raw_bottle_bytes_c;
        port->getEnvelope(stamp);
        mutex.unlock();
    }
};
} // namespace

int Companion::cmdStats(int argc, char *argv[])
{
    BufferedPort<Bottle >  localPort;
    CompanionStatsInput    inData;

    if (argc == 0)
    {
        yCInfo(COMPANION, "This is yarp stats. Syntax:");
        yCInfo(COMPANION, "yarp stats /remote_port");
        yCInfo(COMPANION, "yarp stats /remote_port --duration <time_in_s>");
        return -1;
    }

    Property options;
    options.fromCommand(argc, argv, false);

    //set a callback
    inData.init(localPort);
    localPort.useCallback(inData);

    //open the input port
    localPort.open("...");

    //makes the connection
    std::string remote_port = argv[0];
    std::string protocol="tcp";
    double max_time = options.check("duration", Value(0)).asFloat32();

    if (argc == 2) { protocol = argv[1]; }
    bool b = yarp::os::NetworkBase::connect(remote_port.c_str(), localPort.getName().c_str(), protocol, false);
    if (!b)
    {
        yCInfo(COMPANION) << "Connection failed";
        return -1;
    }

    //compute the statistics
    double start_time=yarp::os::Time::now();
    double period_time=yarp::os::Time::now();
    double total_bottles_received = 0;
    double total_bytes_received = 0;
    size_t periods=0;
    while(true)
    {
        double curr_time= yarp::os::Time::now();
        if (curr_time- period_time >=1.0)
        {
            period_time = yarp::os::Time::now();
            size_t bytes_r=0;
            size_t botts_r=0;
            inData.getStats(bytes_r, botts_r);
            yCInfo(COMPANION) << (bytes_r *8.0/1000000.0) << "Mb/s," << botts_r <<"btl/s," << (1.0 / botts_r) << "mean period (s)";
            total_bytes_received += bytes_r;
            total_bottles_received += botts_r;
            periods++;
            inData.clearStats();
            if (max_time != 0 && (curr_time - start_time) >= max_time)
            {
                break;
            }
        }
        yarp::os::Time::delay(0.001);
    }
    double end_time = yarp::os::Time::now();

    yCInfo(COMPANION) << (total_bytes_received / 1000000) << "total MB received, "<< (total_bottles_received) << "total bottles received," << (periods/total_bottles_received) << "mean period (s)";

    return 0;
}
