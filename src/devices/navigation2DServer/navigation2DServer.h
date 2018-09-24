/* 
 * Copyright (C)2017 ICub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo
 * email:  marco.randazzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/ILocalization2D.h>
#include <math.h>

#ifndef NAV_SERVER_H
#define NAV_SERVER_H

#define DEFAULT_THREAD_PERIOD 0.02 //s

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
    yarp::dev::NavigationStatusEnum   m_navigation_status;
    
    //drivers and interfaces
    yarp::dev::PolyDriver                   pNav;
    yarp::dev::INavigation2DControlActions* iNav_ctrl;
    yarp::dev::INavigation2DTargetActions*  iNav_target;

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
    std::string getStatusAsString(yarp::dev::NavigationStatusEnum status);
};

#endif
