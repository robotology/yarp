/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/PeriodicThread.h>
#include <math.h>
#include <mutex>

#include "FakeLocalizer_ParamsParser.h"

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
    std::mutex                         m_mutex_thread;
    std::string                        m_local_name;

public:
    fakeLocalizerThread(double _period);
    virtual bool threadInit() override;
    virtual void threadRelease() override;
    virtual void run() override;

public:
    bool initializeLocalization(const yarp::dev::Nav2D::Map2DLocation& loc);
    bool getCurrentLoc(yarp::dev::Nav2D::Map2DLocation& loc);
    bool getCurrentLoc(yarp::dev::Nav2D::Map2DLocation& loc, yarp::sig::Matrix& cov);
};

/**
 * @ingroup dev_impl_fake dev_impl_navigation
 *
 * \brief `fakeLocalizer` Documentation to be added
 *
 * Parameters required by this device are shown in class: FakeLocalizer_ParamsParser
 *
 */
class FakeLocalizer :
        public yarp::dev::DeviceDriver,
        public yarp::dev::Nav2D::ILocalization2D,
        public FakeLocalizer_ParamsParser
{
public:
    fakeLocalizerThread         *locThread = nullptr;
    std::mutex                  m_mutex;

    virtual bool open(yarp::os::Searchable& config) override;

    FakeLocalizer();
    virtual ~FakeLocalizer();

    virtual bool close() override;

public:
    //methods inherited from yarp::dev::Nav2D::ILocalization2D
    yarp::dev::ReturnValue   getLocalizationStatus(yarp::dev::Nav2D::LocalizationStatusEnum& status) override;
    yarp::dev::ReturnValue   getEstimatedPoses(std::vector<yarp::dev::Nav2D::Map2DLocation>& poses) override;
    yarp::dev::ReturnValue   getCurrentPosition(yarp::dev::Nav2D::Map2DLocation& loc) override;
    yarp::dev::ReturnValue   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc) override;
    yarp::dev::ReturnValue   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc, const yarp::sig::Matrix& cov) override;
    yarp::dev::ReturnValue   getCurrentPosition(yarp::dev::Nav2D::Map2DLocation& loc, yarp::sig::Matrix& cov) override;
    yarp::dev::ReturnValue   getEstimatedOdometry(yarp::dev::OdometryData& odom)  override;
    yarp::dev::ReturnValue   startLocalizationService() override;
    yarp::dev::ReturnValue   stopLocalizationService() override;
};
