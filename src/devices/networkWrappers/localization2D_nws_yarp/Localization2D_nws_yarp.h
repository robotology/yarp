/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_LOCALIZATION2D_NWS_YARP_H
#define YARP_DEV_LOCALIZATION2D_NWS_YARP_H


#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ILocalization2D.h>
#include <yarp/dev/OdometryData.h>
#include <math.h>

#include "ILocalization2DServerImpl.h"
#include "Localization2D_nws_yarp_ParamsParser.h"

 /**
 * @ingroup dev_impl_nws_yarp dev_impl_navigation
 *
 * \section Localization2D_nws_yarp
 *
 * \brief `localization2D_nws_yarp`: A localization server which can be wrap multiple algorithms and devices to provide robot localization in a 2D World.
 *
 * Parameters required by this device are shown in class: Localization2D_nws_yarp_ParamsParser
 */
class Localization2D_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread,
        public yarp::dev::WrapperSingle,
        public yarp::os::PortReader,
        public Localization2D_nws_yarp_ParamsParser
{
protected:

    //thrift
    ILocalization2DRPCd*                      m_RPC=nullptr;

    //yarp
    std::string                               m_local_name = "/localization2D_nws_yarp";
    yarp::os::Port                            m_rpcPort;
    std::string                               m_rpcPortName;
    yarp::os::BufferedPort<yarp::dev::Nav2D::Map2DLocation>  m_2DLocationPort;
    std::string                               m_2DLocationPortName;
    yarp::os::BufferedPort<yarp::dev::OdometryData>  m_odometryPort;
    std::string                               m_odometryPortName;

    //drivers and interfaces
    yarp::dev::PolyDriver                   pLoc;
    yarp::dev::Nav2D::ILocalization2D*      iLoc = nullptr;
    std::mutex                              m_mutex;

    double                                  m_stats_time_last;
    bool                                    m_getdata_using_periodic_thread=true;

private:
    void publish_2DLocation_on_yarp_port();
    void publish_odometry_on_yarp_port();

public:
    Localization2D_nws_yarp();
    ~Localization2D_nws_yarp() = default;

    bool open(yarp::os::Searchable& prop) override;
    bool close() override;
    bool detach() override;
    bool attach(yarp::dev::PolyDriver* driver) override;
    void run() override;

    bool threadInit() override;
    void threadRelease() override;

    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_DEV_LOCALIZATION2D_NWS_YARP_H
