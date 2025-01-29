/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_ODOMETRY2D_NWS_YARP_H
#define YARP_ODOMETRY2D_NWS_YARP_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Stamp.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IOdometry2D.h>
#include "Odometry2DServerImpl.h"
#include <yarp/dev/WrapperSingle.h>
#include <yarp/os/RpcServer.h>

#include "Odometry2D_nws_yarp_ParamsParser.h"

#define DEFAULT_THREAD_PERIOD 0.02 //s

/**
 * @ingroup dev_impl_nws_yarp dev_impl_navigation
 *
 * \section Odometry2D_nws_yarp_parameters Device description
 * \brief `Odometry2D_nws_yarp`: A yarp nws to get the odometry and publish it on 3 yarp ports:
 *   - a port for odometry;
 *   - a port for odometer;
 *   - a port for velocity.
 * The attached device must implement a `yarp::dev::Nav2D::IOdometry2D` interface.
 *
 * Parameters required by this device are shown in class: Odometry2D_nws_yarp_ParamsParser
 */

class Odometry2D_nws_yarp :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PortReader,
        public Odometry2D_nws_yarp_ParamsParser
{
public:
    Odometry2D_nws_yarp();
    ~Odometry2D_nws_yarp();

    // DeviceDriver
    bool open(yarp::os::Searchable &params) override;
    bool close() override;

    // WrapperSingle
    bool attach(yarp::dev::PolyDriver* driver) override;
    bool detach() override;

    // PeriodicThread
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:
    std::mutex        m_mutex;

    //thrift
    IOdometry2DRPCd* m_RPC=nullptr;

    //rpc port
    bool read(yarp::os::ConnectionReader& connection) override;

    //buffered ports
    yarp::os::BufferedPort<yarp::dev::OdometryData> m_port_odometry;
    yarp::os::BufferedPort<yarp::os::Bottle> m_port_velocity;
    yarp::os::BufferedPort<yarp::os::Bottle> m_port_odometer;
    yarp::os::RpcServer                      m_rpcPort;

    //yarp streaming data
    std::string m_odometerStreamingPortName;
    std::string m_odometryStreamingPortName;
    std::string m_velocityStreamingPortName;
    std::string m_rpcPortName;
    std::string m_deviceName;
    size_t m_stampCount{0};
    yarp::dev::OdometryData m_oldOdometryData{0,0,0,0,0,0,0,0,0};

    // timestamp
    yarp::os::Stamp m_lastStateStamp;

    //interfaces
    yarp::dev::PolyDriver m_driver;
    yarp::dev::Nav2D::IOdometry2D *m_odometry2D_interface{nullptr};

};

#endif // YARP_ODOMETRY2D_NWS_YARP_H
