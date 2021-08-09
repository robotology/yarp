/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef RPLIDAR3_H
#define RPLIDAR3_H


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
 * | GENERAL        | serial_baudrate | int     | -              |   -           | Yes          | Baud rate of the serial port                                       |       |
 * | GENERAL        | sample_buffer_life | int  | -              |   -           | Yes          | Keeps data in memory for some iterations, in order to complete the scan with the missing values (the scan is not always complete)       |  |
 * | GENERAL        | thread_period   | int     | ms             |   0           | No           | Acquisition thread period. The default value = 0 means maximum speed (measured duration ~75ms). It is useful to change it only if you need to slow down the device (e.g. 100ms)    |  |
 * | RPLIDAR        | motor_pwm       | int     | -              |   0           | No           | Used by internal RPLidar APIs                                     |       |
 * | RPLIDAR        | express_mode    | bool    |                | false         | No           | Check sensor datasheet  |  |
 * | RPLIDAR        | force_scan      | bool    |                | false         | No           | Check sensor datasheet  |  |
 * | RPLIDAR        | scan_mode       | string  |                | Boost         | No           | Check sensor datasheet  |  |
 */

class RpLidar3 : public PeriodicThread, public yarp::dev::Lidar2DDeviceBase, public DeviceDriver
{
    typedef rp::standalone::rplidar::RPlidarDriver rplidardrv;

    void                  handleError(u_result error);

protected:
    int                   m_buffer_life = 0;
    bool                  m_inExpressMode = false;
    bool                  m_force_scan = false;
    std::string           m_scan_mode ="Boost";
    int                   m_pwm_val = 600;
    std::string           m_serialPort;
    rplidardrv*           m_drv = nullptr;
    const size_t          m_nodes_num = 8192;
    rplidar_response_measurement_node_hq_t* m_nodes = nullptr;

public:
    RpLidar3(double period = 0) : PeriodicThread(period) //period=0 allows to run the thread as fast as possible, but it is not a busy loop since yield() is called internally
    {
        m_nodes = new rplidar_response_measurement_node_hq_t[m_nodes_num];
    }

    ~RpLidar3()
    {
        delete [] m_nodes;
    }

    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:
    bool startMotor();
    bool startScan();
    bool deviceinfo();

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
