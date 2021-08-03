/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
 *  @ingroup dev_impl_lidar
 *
 * \brief `rpLidar2`: The device driver for the RP2 lidar
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter    | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
 * |:--------------:|:---------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | GENERAL        | serial_port     | string  | -              |   -           | Yes          | Name of the serial port                                           |       |
 * | GENERAL        | serial_baudrate | int     | -              |   -           | Yes          | Baud rate of the srial port                                       |       |
 * | GENERAL        | sample_buffer_life | int  | -              |   -           | Yes          | Keeps data in memory for some iterations, in order to complete the scan with the missing values (the scan is not always complete)       |  |
 * | GENERAL        | motor_pwm       | int     | -              |   0           | No           | Used by internal RPLidar APIs                                     |       |
 * | GENERAL        | thread_period   | int     | ms             |   0           | No           | Acquisition thread period. The default value = 0 means maximum speed (measured duration ~75ms). It is useful to change it only if you need to slow down the device (e.g. 100ms)    |  |
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
    RpLidar2(double period = 0) : PeriodicThread(period), //period=0 allows to run the thead as fast as possibile, but it is not a busy loop since yield() is called internally
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

public:
    //Lidar2DDeviceBase
    bool acquireDataFromHW() override final;
};

#endif
