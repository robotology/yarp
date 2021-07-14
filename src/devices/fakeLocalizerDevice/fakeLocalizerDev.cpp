/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "fakeLocalizerDev.h"

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Node.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/ControlBoardInterfaces.h>

#include <cmath>
#include <random>
#include <mutex>
#include <chrono>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RAD2DEG 180/M_PI
#define DEG2RAD M_PI/180

namespace {
YARP_LOG_COMPONENT(FAKELOCALIZER, "yarp.device.fakeLocalizer")
}

bool   fakeLocalizer::getLocalizationStatus(yarp::dev::Nav2D::LocalizationStatusEnum& status)
{
    status = yarp::dev::Nav2D::LocalizationStatusEnum::localization_status_localized_ok;
    return true;
}

bool   fakeLocalizer::getEstimatedPoses(std::vector<Map2DLocation>& poses)
{
    poses.clear();
    Map2DLocation loc;
    locThread->getCurrentLoc(loc);
    poses.push_back(loc);
#if 0
    //The following block is used only for development and debug purposes.
    //It should be never used in a real scenario
    for (int i = 0; i < 10; i++)
    {
        yarp::dev::Map2DLocation newloc=loc;
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        std::uniform_real_distribution<double> dist(-1, 1);
        std::uniform_real_distribution<double> dist_t(-180, 180);
        double numberx = dist(generator);
        double numbery = dist(generator);
        double numbert = dist_t(generator);
        newloc.x += numberx;
        newloc.y += numbery;
        newloc.theta += numbert;
        poses.push_back(newloc);
    }
#endif
    return true;
}

bool   fakeLocalizer::getCurrentPosition(Map2DLocation& loc)
{
    locThread->getCurrentLoc(loc);
    return true;
}

bool   fakeLocalizer::setInitialPose(const Map2DLocation& loc)
{
    locThread->initializeLocalization(loc);
    return true;
}

bool   fakeLocalizer::getCurrentPosition(Map2DLocation& loc, yarp::sig::Matrix& cov)
{
    locThread->getCurrentLoc(loc);
    return true;
}

bool   fakeLocalizer::setInitialPose(const Map2DLocation& loc, const yarp::sig::Matrix& cov)
{
    locThread->initializeLocalization(loc);
    return true;
}

bool   fakeLocalizer::getEstimatedOdometry(yarp::dev::OdometryData& odom)
{
    Map2DLocation loc;
    locThread->getCurrentLoc(loc);
    odom.odom_x = loc.x;
    odom.odom_y = loc.y;
    odom.odom_theta = loc.theta;
    return true;
}

//////////////////////////

fakeLocalizerThread::fakeLocalizerThread(double _period, yarp::os::Searchable& _cfg) : PeriodicThread(_period), m_cfg(_cfg)
{
    m_last_locdata_received = -1;
    m_last_statistics_printed = -1;

    m_current_loc.map_id = m_current_odom.map_id = m_initial_odom.map_id   = m_initial_loc.map_id = "unknown";
    m_current_loc.x      = m_current_odom.x      = m_initial_odom.x        = m_initial_loc.x = 0;
    m_current_loc.y      = m_current_odom.y      = m_initial_odom.y        = m_initial_loc.y = 0;
    m_current_loc.theta  = m_current_odom.theta  = m_initial_odom.theta    = m_initial_loc.theta = 0;
}

void fakeLocalizerThread::run()
{
    double current_time = yarp::os::Time::now();

    //print some stats every 10 seconds
    if (current_time - m_last_statistics_printed > 10.0)
    {
        m_last_statistics_printed = yarp::os::Time::now();
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    yarp::sig::Vector loc(3);
    loc[0] = 0.0;
    loc[1] = 0.0;
    loc[2] = 0.0;
    if (1)
    {
        m_last_locdata_received = yarp::os::Time::now();
        m_current_odom.x     = loc.data()[0];
        m_current_odom.y     = loc.data()[1];
        m_current_odom.theta = loc.data()[2];

        double c = cos((-m_initial_odom.theta + m_initial_loc.theta)*DEG2RAD);
        double s = sin((-m_initial_odom.theta + m_initial_loc.theta)*DEG2RAD);
        double df_x = (m_current_odom.x - m_initial_odom.x);
        double df_y = (m_current_odom.y - m_initial_odom.y);
        m_current_loc.x = df_x * c + df_y * -s + m_initial_loc.x;
        m_current_loc.y = df_x * s + df_y * +c + m_initial_loc.y;

        m_current_loc.theta = m_current_odom.theta                   - m_initial_odom.theta + m_initial_loc.theta;

        if (m_current_loc.theta >= +360) {
            m_current_loc.theta -= 360;
        } else if (m_current_loc.theta <= -360) {
            m_current_loc.theta += 360;
        }
    }
    if (current_time - m_last_locdata_received > 0.1)
    {
        yCWarning(FAKELOCALIZER) << "No localization data received for more than 0.1s!";
    }
}

bool fakeLocalizerThread::initializeLocalization(const Map2DLocation& loc)
{
    yCInfo(FAKELOCALIZER) << "Localization init request: (" << loc.map_id << ")";
    std::lock_guard<std::mutex> lock(m_mutex);
    m_initial_loc.map_id = loc.map_id;
    m_initial_loc.x = loc.x;
    m_initial_loc.y = loc.y;
    m_initial_loc.theta = loc.theta;
    m_initial_odom.x = m_current_odom.x;
    m_initial_odom.y = m_current_odom.y;
    m_initial_odom.theta = m_current_odom.theta;

    if (m_current_loc.map_id != m_initial_loc.map_id)
    {
        yCInfo(FAKELOCALIZER) << "Map changed from: " << m_current_loc.map_id << " to: " << m_initial_loc.map_id;
        m_current_loc.map_id = m_initial_loc.map_id;
        //@@@TO BE COMPLETED
        m_current_loc.x = 0+m_initial_loc.x;
        m_current_loc.y = 0+m_initial_loc.y;
        m_current_loc.theta = 0+m_initial_loc.theta;
    }
    return true;
}

bool fakeLocalizerThread::getCurrentLoc(Map2DLocation& loc)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    loc = m_current_loc;
    return true;
}

bool fakeLocalizerThread::threadInit()
{
   Map2DLocation loc;
   loc.map_id="test";
   loc.x = 0;
   loc.y = 0;
   loc.theta = 0;
   this->initializeLocalization(loc);
   return true;
}

void fakeLocalizerThread::threadRelease()
{

}


bool fakeLocalizer::open(yarp::os::Searchable& config)
{
    yarp::os::Property p;
    locThread = new fakeLocalizerThread(0.010, p);

    if (!locThread->start())
    {
        delete locThread;
        locThread = NULL;
        return false;
    }

    return true;
}

fakeLocalizer::fakeLocalizer()
{
    locThread = NULL;
}

fakeLocalizer::~fakeLocalizer()
{
    if (locThread)
    {
        delete locThread;
        locThread = NULL;
    }
}

bool fakeLocalizer::startLocalizationService()
{
    return true;
}

bool fakeLocalizer::stopLocalizationService()
{
    return true;
}

bool fakeLocalizer::close()
{
    return true;
}
