/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_ODOMETRY2D_NWC_YARP_H
#define YARP_DEV_ODOMETRY2D_NWC_YARP_H


#include <yarp/os/Network.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IOdometry2D.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>

#include <mutex>

#include "IOdometry2DMsgs.h"
#include "Odometry2D_nwc_yarp_ParamsParser.h"

class OdometryData2DInputPortProcessor :
        public yarp::os::BufferedPort<yarp::dev::OdometryData>
{
    const int DATA_TIMEOUT_ms=100; //ms

    yarp::dev::OdometryData lastScan;
    std::mutex mutex;
    yarp::os::Stamp lastStamp;
    double deltaT;
    double deltaTMax;
    double deltaTMin;
    double prev;
    double now;

    int count;

public:
    OdometryData2DInputPortProcessor();
    using yarp::os::BufferedPort<yarp::dev::OdometryData>::onRead;
    void onRead(yarp::dev::OdometryData& v) override;

public:
    bool timeout_occurred = false;
    inline void resetStat();
    inline bool getLast(yarp::dev::OdometryData& data, yarp::os::Stamp& stmp);
    inline int getIterations();
    // time is in ms
    void getEstFrequency(int &ite, double &av, double &min, double &max);
};

/**
* @ingroup dev_impl_nwc_yarp dev_impl_navigation
*
* \brief `Odometry2D_nwc_yarp`: The client side of any IOdometry2D capable device.
*
* Parameters required by this device are shown in class: Odometry2D_nwc_yarp_ParamsParser
*
*/
class Odometry2D_nwc_yarp:
        public yarp::dev::DeviceDriver,
        public yarp::dev::Nav2D::IOdometry2D,
        public Odometry2D_nwc_yarp_ParamsParser
{
protected:
    OdometryData2DInputPortProcessor m_inputPort;
    IOdometry2DMsgs                  m_RPC;
    std::mutex                       m_mutex;
    yarp::os::Port                   m_rpcPort;
    yarp::os::Stamp                  m_lastTs;

public:
    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* IOdometry2D methods */
    virtual yarp::dev::ReturnValue   getOdometry(yarp::dev::OdometryData& odom, double* timestamp = nullptr) override;
    virtual yarp::dev::ReturnValue   resetOdometry() override;
};

#endif // YARP_DEV_ODOMETRY2D_NWC_YARP_H
