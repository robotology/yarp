/*
•   Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT)
•   All rights reserved.
•
•   This software may be modified and distributed under the terms of the
•   GPL-2+ license. See the accompanying LICENSE file for details.
*/

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/Mutex.h>
#include <yarp/os/LockGuard.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Node.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/PeriodicThread.h>
#include <math.h>

using namespace yarp::os;

namespace yarp
{
    namespace dev
    {
        class fakeLocalizerThread;
        class fakeLocalizer;
    }
}

class yarp::dev::fakeLocalizer : public yarp::dev::DeviceDriver,
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
    bool   getEstimatedPoses(std::vector<yarp::dev::Map2DLocation>& poses) override;

    /**
    * Gets the current position of the robot w.r.t world reference frame
    * @param loc the location of the robot
    * @return true/false
    */
    bool   getCurrentPosition(yarp::dev::Map2DLocation& loc) override;

    /**
    * Sets the initial pose for the localization algorithm which estimates the current position of the robot w.r.t world reference frame.
    * @param loc the location of the robot
    * @return true/false
    */
    bool   setInitialPose(yarp::dev::Map2DLocation& loc) override;
};

class yarp::dev::fakeLocalizerThread : public yarp::os::PeriodicThread
{
protected:
    //general
    double                       m_last_statistics_printed;
    double                       m_last_locdata_received;
    yarp::dev::Map2DLocation     m_initial_loc;
    yarp::dev::Map2DLocation     m_initial_odom;
    yarp::dev::Map2DLocation     m_current_loc;
    yarp::dev::Map2DLocation     m_current_odom;
    yarp::os::Mutex              m_mutex;
    yarp::os::Searchable&        m_cfg;
    std::string                  m_local_name;

public:
    fakeLocalizerThread(double _period, yarp::os::Searchable& _cfg);
    virtual bool threadInit() override;
    virtual void threadRelease() override;
    virtual void run() override;

public:
    bool initializeLocalization(yarp::dev::Map2DLocation& loc);
    bool getCurrentLoc(yarp::dev::Map2DLocation& loc);
};
