/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/ILocalization2D.h>
#include "ILocalization2DServerImpl.h"

/*! \file ILocalization2DServerImpl.cpp */

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace std;

#define DEFAULT_THREAD_PERIOD 0.01

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace {
YARP_LOG_COMPONENT(LOCALIZATION2DSERVER, "yarp.device.localization2DServer")
}

#define CHECK_POINTER(xxx) {if (xxx==nullptr) {yCError(LOCALIZATION2DSERVER, "Invalid interface"); return false;}}

bool ILocalization2DRPCd::start_localization_service_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iLoc == nullptr) { yCError(LOCALIZATION2DSERVER, "Invalid interface"); return false; }}

    if (!m_iLoc->startLocalizationService())
    {
        yCError(LOCALIZATION2DSERVER, "Unable to startLocalizationService");
        return false;
    }
    return true;
}

bool ILocalization2DRPCd::stop_localization_service_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iLoc == nullptr) { yCError(LOCALIZATION2DSERVER, "Invalid interface"); return false; }}

    if (!m_iLoc->stopLocalizationService())
    {
        yCError(LOCALIZATION2DSERVER, "Unable to stopLocalizationService");
        return false;
    }
    return true;
}

return_get_localization_status ILocalization2DRPCd::get_localization_status_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_localization_status ret;
    if (m_getdata_using_periodic_thread)
    {
        ret.ret = true;
        ret.status = yarp::dev::Nav2D::LocalizationStatusEnum(m_current_status);
        return ret;
    }

    {if (m_iLoc == nullptr) { yCError(LOCALIZATION2DSERVER, "Invalid interface"); return ret; }}

    yarp::dev::Nav2D::LocalizationStatusEnum status;
    if (!m_iLoc->getLocalizationStatus(status))
    {
        yCError(LOCALIZATION2DSERVER, "Unable to getLocalizationStatus");
        ret.ret=false;
        ret.status = yarp::dev::Nav2D::LocalizationStatusEnum(yarp::dev::Nav2D::LocalizationStatusEnum::localization_status_error) ;
    }
    else
    {
        ret.ret=true;
        ret.status = yarp::dev::Nav2D::LocalizationStatusEnum(status);
    }
    return ret;
}

return_get_estimated_poses ILocalization2DRPCd::get_estimated_poses_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_estimated_poses ret;
    {if (m_iLoc == nullptr) { yCError(LOCALIZATION2DSERVER, "Invalid interface"); return ret; }}
    std::vector<yarp::dev::Nav2D::Map2DLocation> poses;
    if (!m_iLoc->getEstimatedPoses(poses))
    {
        yCError(LOCALIZATION2DSERVER, "Unable to getEstimatedPoses");
        ret.ret = false;
    }
    else
    {
        ret.ret = true;
        ret.poses=poses;
    }
    return ret;
}

return_get_current_position1 ILocalization2DRPCd::get_current_position1_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_current_position1 ret;
    if (m_getdata_using_periodic_thread)
    {
        ret.ret = true;
        ret.loc = this->m_current_position;
        return ret;
    }

    {if (m_iLoc == nullptr) { yCError(LOCALIZATION2DSERVER, "Invalid interface"); return ret; }}

    yarp::dev::Nav2D::Map2DLocation pos;
    if (!m_iLoc->getCurrentPosition(pos))
    {
        yCError(LOCALIZATION2DSERVER, "Unable to getCurrentPosition");
        ret.ret = false;
    }
    else
    {
        ret.ret = true;
        ret.loc = pos;
    }
    return ret;
}

return_get_current_position2 ILocalization2DRPCd::get_current_position2_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_current_position2 ret;
    {if (m_iLoc == nullptr) { yCError(LOCALIZATION2DSERVER, "Invalid interface"); return ret; }}
    yarp::dev::Nav2D::Map2DLocation pos;
    yarp::sig::Matrix cov;
    if (!m_iLoc->getCurrentPosition(pos,cov))
    {
        yCError(LOCALIZATION2DSERVER, "Unable to getCurrentPosition");
        ret.ret = false;
    }
    else
    {
        ret.ret = true;
        ret.loc=pos;
        ret.cov=cov;
    }
    return ret;
}

return_get_estimated_odometry ILocalization2DRPCd::get_estimated_odometry_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_estimated_odometry ret;
    if (m_getdata_using_periodic_thread)
    {
        ret.ret = true;
        ret.odom = this->m_current_odometry;
        return ret;
    }

    {if (m_iLoc == nullptr) { yCError(LOCALIZATION2DSERVER, "Invalid interface"); return ret; }}

    yarp::dev::OdometryData odom;
    if (!m_iLoc->getEstimatedOdometry(odom))
    {
        yCError(LOCALIZATION2DSERVER, "Unable to getEstimatedOdometry");
        ret.ret = false;
    }
    else
    {
        ret.ret = true;
        ret.odom = odom;
    }
    return ret;
}

bool ILocalization2DRPCd::set_initial_pose1_RPC(const yarp::dev::Nav2D::Map2DLocation& loc)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iLoc == nullptr) { yCError(LOCALIZATION2DSERVER, "Invalid interface"); return false; }}
    if (!m_iLoc->setInitialPose(loc))
    {
        yCError(LOCALIZATION2DSERVER, "Unable to setInitialPose");
        return false;
    }
    return true;
}

bool ILocalization2DRPCd::set_initial_pose2_RPC(const yarp::dev::Nav2D::Map2DLocation& loc, const yarp::sig::Matrix& cov)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iLoc == nullptr) { yCError(LOCALIZATION2DSERVER, "Invalid interface"); return false; }}

    if (!m_iLoc->setInitialPose(loc,cov))
    {
        yCError(LOCALIZATION2DSERVER, "Unable to setInitialPose");
        return false;
    }
    return true;
}
