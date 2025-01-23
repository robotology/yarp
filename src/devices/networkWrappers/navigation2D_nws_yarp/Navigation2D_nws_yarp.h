/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/INavigation2D.h>
#include "INavigation2DServerImpl.h"
#include <math.h>

#include "Navigation2D_nws_yarp_ParamsParser.h"

#ifndef NAV_SERVER_H
#define NAV_SERVER_H

#define DEFAULT_THREAD_PERIOD 0.02 //s

  /**
  * @ingroup dev_impl_nws_yarp dev_impl_navigation
  *
  * \section Navigation2D_nws_yarp
  *
  * \brief `navigation2D_nws_yarp`: A navigation server which can be wrap multiple algorithms and devices to perform a navigation task in a 2D World.
  *
  * Parameters required by this device are shown in class: Navigation2D_nws_yarp_ParamsParser
  *
  */

#define DEF_m_RPC 1

class Navigation2D_nws_yarp : public yarp::dev::DeviceDriver,
                         public yarp::os::PeriodicThread,
                         public yarp::dev::WrapperSingle,
                         public yarp::os::PortReader,
                         public Navigation2D_nws_yarp_ParamsParser
{
protected:
    //thrift
    INavigation2DRPCd*                           m_RPC=nullptr;

    yarp::os::Port                               m_rpcPort;
    yarp::os::Port                               m_statusPort;
    std::string                                  m_rpcPortName;
    std::string                                  m_statusPortName;
    yarp::dev::Nav2D::NavigationStatusEnum       m_navigation_status;
    std::string                                  m_prev_navigation_status;

    //drivers and interfaces
    yarp::dev::PolyDriver                            pNav;
    yarp::dev::Nav2D::INavigation2DControlActions*   iNav_ctrl = nullptr;
    yarp::dev::Nav2D::INavigation2DTargetActions*    iNav_target = nullptr;
    yarp::dev::Nav2D::INavigation2DVelocityActions*  iNav_vel = nullptr;

    double                                  m_stats_time_last;

public:
    /**
    * Default module constructor
    */
    Navigation2D_nws_yarp();

public:
    virtual bool open(yarp::os::Searchable& prop) override;
    virtual bool close() override;
    virtual bool detach() override;
    virtual bool attach(yarp::dev::PolyDriver* drv) override;
    virtual void run() override;

    bool initialize_YARP(yarp::os::Searchable &config);
    virtual bool read(yarp::os::ConnectionReader& connection) override;

private:
    std::string getStatusAsString(yarp::dev::Nav2D::NavigationStatusEnum status);
    void updateStatusPort(std::string& status);
};

#endif
