/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef LASER_FROM_EXTERNAL_PORT_H
#define LASER_FROM_EXTERNAL_PORT_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/LaserScan2D.h>
#include <yarp/dev/Lidar2DDeviceBase.h>
#include <yarp/sig/Vector.h>


#include <mutex>
#include <string>
#include <vector>

using namespace yarp::os;
using namespace yarp::dev;

typedef unsigned char byte;

//---------------------------------------------------------------------------------------------------------------

class InputPortProcessor :
    public yarp::os::BufferedPort<yarp::dev::LaserScan2D>
{
    std::mutex             mutex;
    yarp::dev::LaserScan2D lastScan;
    yarp::os::Stamp        lastStamp;

public:

    InputPortProcessor();
    using yarp::os::BufferedPort<yarp::dev::LaserScan2D>::onRead;
    void onRead(yarp::dev::LaserScan2D& v) override;
    void getLast(yarp::dev::LaserScan2D& data, yarp::os::Stamp& stmp);
};

class LaserFromExternalPort : public yarp::dev::Lidar2DDeviceBase,
                              public PeriodicThread,
                              public DeviceDriver
{
protected:
    std::string        m_port_name;
    InputPortProcessor m_input_port;

    yarp::os::Stamp        m_last_stamp;
    yarp::dev::LaserScan2D m_last_scan_data;


public:
    LaserFromExternalPort(double period = 0.01) : PeriodicThread(period), Lidar2DDeviceBase()
    {}

    ~LaserFromExternalPort()
    {
    }

    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

public:
    //IRangefinder2D interface
    bool setDistanceRange        (double min, double max) override;
    bool setScanLimits           (double min, double max) override;
    bool setHorizontalResolution (double step) override;
    bool setScanRate             (double rate) override;
};

#endif
