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
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/ILocalization2D.h>
#include <math.h>

#ifndef NAV_SERVER_H
#define NAV_SERVER_H

#define DEFAULT_THREAD_PERIOD 0.02 //s

 /**
 * @ingroup dev_impl_network_wrapper dev_impl_navigation
  *
  * \brief `navigation2DServer`: Documentation to be added
  */
class navigation2DServer : public yarp::dev::DeviceDriver,
                         public yarp::os::PeriodicThread,
                         public yarp::dev::IMultipleWrapper,
//                         public yarp::dev::INavigation2DTargetActions,
//                         public yarp::dev::INavigation2DControlActions,
                         public yarp::os::PortReader
{
protected:
//    yarp::os::BufferedPort<yarp::os::Bottle>   m_yarpview_target_Port;
    yarp::os::Port                    m_rpcPort;
    std::string                       m_rpcPortName;
    std::string                       m_streamingPortName;
    std::string                       m_yarpviewPortName;
    yarp::dev::Nav2D::NavigationStatusEnum   m_navigation_status;

    //drivers and interfaces
    yarp::dev::PolyDriver                   pNav;
    yarp::dev::Nav2D::INavigation2DControlActions* iNav_ctrl;
    yarp::dev::Nav2D::INavigation2DTargetActions*  iNav_target;

    double                                  m_period;
    double                                  m_stats_time_last;

public:
    /**
    * Default module constructor
    */
    navigation2DServer();

public:
    virtual bool open(yarp::os::Searchable& prop) override;
    virtual bool close() override;
    virtual bool detachAll() override;
    virtual bool attachAll(const yarp::dev::PolyDriverList &l) override;
    virtual void run() override;

    bool initialize_YARP(yarp::os::Searchable &config);
    virtual bool read(yarp::os::ConnectionReader& connection) override;

private:
    std::string getStatusAsString(yarp::dev::Nav2D::NavigationStatusEnum status);
    bool parse_respond_string(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
    bool parse_respond_vocab(const yarp::os::Bottle& command, yarp::os::Bottle& reply);

private:
    std::string m_current_goal_name;
    bool set_current_goal_name(const std::string& name);
    bool get_current_goal_name(std::string& name);
    bool clear_current_goal_name();
};

#endif
