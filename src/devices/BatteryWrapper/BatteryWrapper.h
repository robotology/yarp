/*
 * Copyright (C) 2015 Istituto Italiano di Tecnologia (IIT)
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */

#ifndef YARP_DEV_BATTERYWRAPPER_BATTERYWRAPPER_H
#define YARP_DEV_BATTERYWRAPPER_BATTERYWRAPPER_H

 //#include <list>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h>

#include <yarp/os/RateThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Stamp.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/IBattery.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/api.h>


/* Using yarp::dev::impl namespace for all helper class inside yarp::dev to reduce
 * name conflicts
 */

namespace yarp{
    namespace dev{
        class BatteryWrapper;
        }
}

#define DEFAULT_THREAD_PERIOD 20 //ms

class yarp::dev::BatteryWrapper: public yarp::os::RateThread,
                                public yarp::dev::DeviceDriver,
                                public yarp::dev::IMultipleWrapper,
                                public yarp::os::PortReader
{
public:
    BatteryWrapper();
    ~BatteryWrapper();

    bool open(yarp::os::Searchable &params) override;
    bool close() override;
    yarp::os::Bottle getOptions();

    void setId(const std::string &id);
    std::string getId();

    /**
      * Specify which analog sensor this thread has to read from.
      */
    bool attachAll(const PolyDriverList &p) override;
    bool detachAll() override;

    void attach(yarp::dev::IBattery *s);
    void detach();

    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    yarp::os::ConstString streamingPortName;
    yarp::os::ConstString rpcPortName;
    yarp::os::Port rpcPort;
    yarp::os::BufferedPort<yarp::os::Bottle> streamingPort;
    yarp::dev::IBattery *battery_p;             // the battery read from
    yarp::os::Stamp lastStateStamp;             // the last reading time stamp
    int _rate;
    std::string sensorId;

    bool initialize_YARP(yarp::os::Searchable &config);
    virtual bool read(yarp::os::ConnectionReader& connection) override;

#endif //DOXYGEN_SHOULD_SKIP_THIS
};

#endif // YARP_DEV_BATTERYWRAPPER_BATTERYWRAPPER_H
