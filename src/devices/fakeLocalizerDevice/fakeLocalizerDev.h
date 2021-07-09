/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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

/**
 * @ingroup dev_impl_fake dev_impl_navigation
 *
 * \brief `fakeLocalizer` Documentation to be added
 */
class fakeLocalizer :
        public yarp::dev::DeviceDriver,
        public yarp::dev::Nav2D::ILocalization2D
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
    bool   getLocalizationStatus(yarp::dev::Nav2D::LocalizationStatusEnum& status) override;

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

    /**
    * Sets the initial pose for the localization algorithm which estimates the current position of the robot w.r.t world reference frame.
    * @param loc the location of the robot
    * @param cov the 3x3 covariance matrix
    * @return true/false
    */
    virtual bool   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc, const yarp::sig::Matrix& cov) override;

    /**
    * Gets the current position of the robot w.r.t world reference frame, plus the covariance
    * @param loc the location of the robot
    * @param cov the 3x3 covariance matrix
    * @return true/false
    */
    virtual bool   getCurrentPosition(yarp::dev::Nav2D::Map2DLocation& loc, yarp::sig::Matrix& cov) override;

    /**
    * Gets the estimated odometry the robot, including its velocity expressed in the world and in the local reference frame.
    * @param loc the estimated odometry.
    * @return true/false
    */
    virtual bool   getEstimatedOdometry(yarp::dev::OdometryData& odom)  override;

    /**
    * Starts the localization service
    * @return true/false
    */
    virtual bool   startLocalizationService() override;

    /**
    * Stops the localization service
    * @return true/false
    */
    virtual bool   stopLocalizationService() override;
};
