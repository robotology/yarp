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

#ifndef RPLIDAR2_H
#define RPLIDAR2_H


#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/Lidar2DDeviceBase.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/ISerialDevice.h>
#include <yarp/dev/Lidar2DDeviceBase.h>

#include <mutex>
#include <string>
#include <vector>

#include <rplidar.h>

using namespace yarp::os;
using namespace yarp::dev;

typedef unsigned char byte;

//---------------------------------------------------------------------------------------------------------------
/**
 * @ingroup dev_impl_lidar
 *
 * Documentation to be added
 */
class RpLidar2 : public PeriodicThread, public yarp::dev::Lidar2DDeviceBase, public DeviceDriver
{
    typedef rp::standalone::rplidar::RPlidarDriver rplidardrv;

    void                  handleError(u_result error);
    std::string deviceinfo();
protected:
    int                   m_buffer_life;
    bool                  m_inExpressMode;
    int                   m_pwm_val;
    std::string           m_serialPort;
    rplidardrv*           m_drv;

public:
    RpLidar2(double period = 0.01) : PeriodicThread(period),
        m_buffer_life(0),
        m_inExpressMode(false),
        m_pwm_val(0),
        m_drv(nullptr)
    {}


    ~RpLidar2()
    {
    }

    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

public:
    //IRangefinder2D interface
    bool setDistanceRange     (double min, double max) override;
    bool setScanLimits        (double min, double max) override;
    bool setHorizontalResolution      (double step) override;
    bool setScanRate          (double rate) override;
};

#endif
