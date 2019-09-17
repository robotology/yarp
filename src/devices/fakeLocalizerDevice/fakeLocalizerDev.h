/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
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

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Node.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/PeriodicThread.h>
#include <math.h>
#include <mutex>

using namespace yarp::os;


class fakeLocalizerThread :
        public yarp::os::PeriodicThread
{
protected:
    //general
    double                             m_last_statistics_printed;
    double                             m_last_locdata_received;
    yarp::dev::Nav2D::Map2DLocation    m_initial_loc;
    yarp::dev::Nav2D::Map2DLocation    m_initial_odom;
    yarp::dev::Nav2D::Map2DLocation    m_current_loc;
    yarp::dev::Nav2D::Map2DLocation    m_current_odom;
    std::mutex                         m_mutex;
    yarp::os::Searchable&              m_cfg;
    std::string                        m_local_name;

public:
    fakeLocalizerThread(double _period, yarp::os::Searchable& _cfg);
    virtual bool threadInit() override;
    virtual void threadRelease() override;
    virtual void run() override;

public:
    bool initializeLocalization(const yarp::dev::Nav2D::Map2DLocation& loc);
    bool getCurrentLoc(yarp::dev::Nav2D::Map2DLocation& loc);
};


class fakeLocalizer :
        public yarp::dev::DeviceDriver,
        public yarp::dev::ILocalization2D
{
public:
    fakeLocalizerThread         *locThread;
    virtual bool open(yarp::os::Searchable& config) override;

    fakeLocalizer();
    virtual ~fakeLocalizer();

    virtual bool close() override;

public:
    /**
    * Gets the current status of the localization task.
    * @return true/false
    */
    bool   getLocalizationStatus(yarp::dev::LocalizationStatusEnum& status) override;

    /**
    * Gets a set of pose estimates computed by the localization algorithm.
    * @return true/false
    */
    bool   getEstimatedPoses(std::vector<yarp::dev::Nav2D::Map2DLocation>& poses) override;

    /**
    * Gets the current position of the robot w.r.t world reference frame
    * @param loc the location of the robot
    * @return true/false
    */
    bool   getCurrentPosition(yarp::dev::Nav2D::Map2DLocation& loc) override;

    /**
    * Sets the initial pose for the localization algorithm which estimates the current position of the robot w.r.t world reference frame.
    * @param loc the location of the robot
    * @return true/false
    */
    bool   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc) override;
};
