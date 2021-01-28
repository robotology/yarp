/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
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

#ifndef LASER_FROM_DEPTH_H
#define LASER_FROM_DEPTH_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/Lidar2DDeviceBase.h>

#include <mutex>
#include <string>
#include <vector>

using namespace yarp::os;
using namespace yarp::dev;

typedef unsigned char byte;

//---------------------------------------------------------------------------------------------------------------
/**
 * @ingroup dev_impl_lidar
 *
 * \brief `laserFromDepth`: Documentation to be added
 */
class LaserFromDepth : public PeriodicThread, public yarp::dev::Lidar2DDeviceBase, public DeviceDriver
{
protected:
    PolyDriver driver;
    IRGBDSensor* iRGBD = nullptr;

    size_t m_depth_width = 0;
    size_t m_depth_height = 0;
    yarp::sig::ImageOf<float> m_depth_image;

public:
    LaserFromDepth(double period = 0.01) : PeriodicThread(period),
        Lidar2DDeviceBase()
    {}

    ~LaserFromDepth()
    {
    }

    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

public:
    //IRangefinder2D interface
    bool setDistanceRange    (double min, double max) override;
    bool setScanLimits        (double min, double max) override;
    bool setHorizontalResolution      (double step) override;
    bool setScanRate         (double rate) override;

public:
    //Lidar2DDeviceBase
    bool acquireDataFromHW() override final;
};

#endif
