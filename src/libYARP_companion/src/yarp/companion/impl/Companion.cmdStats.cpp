/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
using yarp::os::Port;
using yarp::os::Contactable;
using yarp::os::LogStream;
using yarp::os::NetworkBase;
using yarp::os::Property;
using yarp::os::Semaphore;
using yarp::os::Stamp;
using yarp::os::TypedReaderCallback;
using yarp::os::Value;
using yarp::os::PortReader;
using yarp::os::ConnectionReader;

namespace {

class CompanionStatsInput
{
public:
    Contactable* port = nullptr;
    std::mutex mutex;

    virtual ~CompanionStatsInput() {}

protected:
    Bottle value;
    size_t bottles_received = 0;
    size_t bytes_received = 0;
    Stamp  stamp;

public:
    virtual void clearStats()
    {
        mutex.lock();
        bottles_received = 0;
        bytes_received = 0;
        mutex.unlock();
    }

    virtual void getStats(size_t& b1, size_t& b2)
    {
        mutex.lock();
        b1 = bytes_received;
        b2 = bottles_received;
        mutex.unlock();
    }

    virtual void init(Contactable* port)
    {
        this->port = port;
    }
};

class CompanionStatsInputPort : public PortReader,
                                public CompanionStatsInput
{
public:
    CompanionStatsInputPort() = default;

    using yarp::os::PortReader::read;
    bool read(ConnectionReader& connection) override
    {
        Bottle datum;
        bool ok = datum.read(connection);
        if (!ok)
        {
            return false;
        }
        // process data in b
        mutex.lock();
        value = datum;
        bottles_received++;
        size_t raw_bottle_bytes_c = 0;
        const char* raw_bottle_bytes_p = value.toBinary(&raw_bottle_bytes_c);
        YARP_UNUSED(raw_bottle_bytes_p);
        bytes_received += raw_bottle_bytes_c;
        port->getEnvelope(stamp);
        mutex.unlock();
        return true;
    }
};

class CompanionStatsInputBufferedPort : public TypedReaderCallback<Bottle>,
                                        public CompanionStatsInput
{
public:
    CompanionStatsInputBufferedPort() = default;

    using yarp::os::TypedReaderCallback<Bottle>::onRead;
    void onRead(Bottle& datum) override
    {
        mutex.lock();
        value = datum;
        bottles_received++;
        size_t raw_bottle_bytes_c=0;
        const char* raw_bottle_bytes_p = value.toBinary(&raw_bottle_bytes_c);
        YARP_UNUSED(raw_bottle_bytes_p);
        bytes_received += raw_bottle_bytes_c;
        port->getEnvelope(stamp);
        mutex.unlock();
    }
};
} // namespace

int Companion::cmdStats(int argc, char *argv[])
{
    Contactable*            localPort =nullptr;
    CompanionStatsInput*    inData=nullptr;

    if (argc == 0)
    {
        yCInfo(COMPANION, "This is yarp stats. Syntax:");
        yCInfo(COMPANION, "yarp stats /remote_port");
        yCInfo(COMPANION, "yarp stats /remote_port --port_type BufferedPort");
        yCInfo(COMPANION, "yarp stats /remote_port --port_type Port");
        yCInfo(COMPANION, "yarp stats /remote_port --duration <time_in_s> --protocol <protocol_name>");
        yCInfo(COMPANION);
        yCInfo(COMPANION, "If port_type is not specified, BufferedPort is assumed as default");
        return -1;
    }

    Property options;
    options.fromCommand(argc, argv, false);

    std::string port_type = options.check("port_type", Value("BufferedPort")).asString();

    //set a callback
    if (port_type == "BufferedPort")
    {
        //buffered port implementation
        localPort=new BufferedPort<Bottle>;
        inData = new CompanionStatsInputBufferedPort;
        inData->init(localPort);
        BufferedPort<Bottle>* pp= dynamic_cast<BufferedPort<Bottle>*>(localPort);
        pp->useCallback(*dynamic_cast<CompanionStatsInputBufferedPort*>(inData));
        yCInfo(COMPANION) << "Using BufferedPort for connection";
    }
    else if (port_type == "Port")
    {
        //standard port implementation
        localPort = new Port;
        inData = new CompanionStatsInputPort;
        inData->init(localPort);
        localPort->setReader(*dynamic_cast<CompanionStatsInputPort*>(inData));
        yCInfo(COMPANION) << "Using standard Port for connection";
    }
    else
    {
        yCInfo(COMPANION) << "Invalid value for parameter port_type. Must be `BufferedPort` or `Port`";
        return -1;
    }

    //open the input port
    localPort->open("...");

    //makes the connection
    std::string remote_port = argv[0];
    std::string protocol= options.check("protocol", Value("tcp")).asString();
    double max_time = options.check("duration", Value(0)).asFloat32();

    bool b = yarp::os::NetworkBase::connect(remote_port.c_str(), localPort->getName().c_str(), protocol, false);
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
            inData->getStats(bytes_r, botts_r);
            yCInfo(COMPANION) << (bytes_r *8.0/1000000.0) << "Mb/s," << botts_r <<"btl/s," << (1.0 / botts_r) << "mean period (s)";
            total_bytes_received += bytes_r;
            total_bottles_received += botts_r;
            periods++;
            inData->clearStats();
            if (max_time != 0 && (curr_time - start_time) >= max_time)
            {
                break;
            }
        }
        yarp::os::Time::delay(0.001);
    }
    // double end_time = yarp::os::Time::now();

    yCInfo(COMPANION) << (total_bytes_received / 1000000) << "total MB received, "<< (total_bottles_received) << "total bottles received," << (periods/total_bottles_received) << "mean period (s)";

    localPort->interrupt();
    localPort->close();
    delete localPort;
    delete inData;

    return 0;
}
