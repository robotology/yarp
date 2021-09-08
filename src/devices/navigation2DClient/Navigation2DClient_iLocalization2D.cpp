/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Navigation2DClient.h"
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <cmath>

 /*! \file Navigation2DClient.cpp */

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
    YARP_LOG_COMPONENT(NAVIGATION2DCLIENT, "yarp.device.navigation2DClient")
}

//------------------------------------------------------------------------------------------------------------------------------

bool  Navigation2DClient::setInitialPose(const Map2DLocation& loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_SET_INITIAL_POS);
    b.addString(loc.map_id);
    b.addFloat64(loc.x);
    b.addFloat64(loc.y);
    b.addFloat64(loc.theta);

    bool ret = m_rpc_port_to_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "setInitialPose() received error from localization server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "setInitialPose() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::setInitialPose(const Map2DLocation& loc, const yarp::sig::Matrix& cov)
{
    if (cov.rows() != 3 || cov.cols() != 3)
    {
        yCError(NAVIGATION2DCLIENT) << "Covariance matrix is expected to have size (3,3)";
        return false;
    }
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_SET_INITIAL_POSCOV);
    b.addString(loc.map_id);
    b.addFloat64(loc.x);
    b.addFloat64(loc.y);
    b.addFloat64(loc.theta);
    yarp::os::Bottle& mc = b.addList();
    for (int i = 0; i < 3; i++) { for (int j = 0; j < 3; j++) { mc.addFloat64(cov[i][j]); } }

    bool ret = m_rpc_port_to_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "setInitialPose() received error from localization server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "setInitialPose() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  Navigation2DClient::getCurrentPosition(Map2DLocation& loc, yarp::sig::Matrix& cov)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_CURRENT_POSCOV);

    bool ret = m_rpc_port_to_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK || resp.size() != 6)
        {
            yCError(NAVIGATION2DCLIENT) << "getCurrentPosition() received error from localization server";
            return false;
        }
        else
        {
            if (cov.rows() != 3 || cov.cols() != 3)
            {
                yCDebug(NAVIGATION2DCLIENT) << "Performance warning: covariance matrix is not (3,3), resizing...";
                cov.resize(3, 3);
            }
            loc.map_id = resp.get(1).asString();
            loc.x = resp.get(2).asFloat64();
            loc.y = resp.get(3).asFloat64();
            loc.theta = resp.get(4).asFloat64();
            Bottle* mc = resp.get(5).asList();
            if (mc == nullptr) {
                return false;
            }
            for (size_t i = 0; i < 3; i++) { for (size_t j = 0; j < 3; j++) { cov[i][j] = mc->get(i * 3 + j).asFloat64(); } }
            return true;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "getCurrentPosition() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  Navigation2DClient::getEstimatedOdometry(yarp::dev::OdometryData& odom)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_ESTIMATED_ODOM);

    bool ret = m_rpc_port_to_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK || resp.size() != 10)
        {
            yCError(NAVIGATION2DCLIENT) << "getEstimatedOdometry() received error from localization server";
            return false;
        }
        else
        {
            odom.odom_x = resp.get(1).asFloat64();
            odom.odom_y = resp.get(2).asFloat64();
            odom.odom_theta = resp.get(3).asFloat64();
            odom.base_vel_x = resp.get(4).asFloat64();
            odom.base_vel_y = resp.get(5).asFloat64();
            odom.base_vel_theta = resp.get(6).asFloat64();
            odom.odom_vel_x = resp.get(7).asFloat64();
            odom.odom_vel_y = resp.get(8).asFloat64();
            odom.odom_vel_theta = resp.get(9).asFloat64();
            return true;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "getEstimatedOdometry() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  Navigation2DClient::getCurrentPosition(Map2DLocation& loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_CURRENT_POS);

    bool ret = m_rpc_port_to_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK || resp.size() != 5)
        {
            yCError(NAVIGATION2DCLIENT) << "getCurrentPosition() received error from localization server";
            return false;
        }
        else
        {
            loc.map_id = resp.get(1).asString();
            loc.x = resp.get(2).asFloat64();
            loc.y = resp.get(3).asFloat64();
            loc.theta = resp.get(4).asFloat64();
            return true;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "getCurrentPosition() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  Navigation2DClient::getLocalizationStatus(yarp::dev::Nav2D::LocalizationStatusEnum& status)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_LOCALIZER_STATUS);

    bool ret = m_rpc_port_to_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK || resp.size() != 2)
        {
            yCError(NAVIGATION2DCLIENT) << "getLocalizationStatus() received error from localization server";
            return false;
        }
        else
        {
            status = (yarp::dev::Nav2D::LocalizationStatusEnum)(resp.get(1).asVocab32());
            return true;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "getLocalizationStatus() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  Navigation2DClient::getEstimatedPoses(std::vector<Map2DLocation>& poses)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_LOCALIZER_POSES);

    bool ret = m_rpc_port_to_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "getEstimatedPoses() received error from localization server";
            return false;
        }
        else
        {
            int nposes = resp.get(1).asInt32();
            poses.clear();
            for (int i = 0; i < nposes; i++)
            {
                Map2DLocation loc;
                Bottle* b = resp.get(2 + i).asList();
                if (b)
                {
                    loc.map_id = b->get(0).asString();
                    loc.x = b->get(1).asFloat64();
                    loc.y = b->get(2).asFloat64();
                    loc.theta = b->get(3).asFloat64();
                }
                else
                {
                    poses.clear();
                    yCError(NAVIGATION2DCLIENT) << "getEstimatedPoses() parsing error";
                    return false;
                }
                poses.push_back(loc);
            }
            return true;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "getEstimatedPoses() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  Navigation2DClient::startLocalizationService()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_LOCALIZATION_START);

    bool ret = m_rpc_port_to_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "startLocalizationService() received error from navigation server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "startLocalizationService() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  Navigation2DClient::stopLocalizationService()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_LOCALIZATION_STOP);

    bool ret = m_rpc_port_to_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "stopLocalizationService() received error from navigation server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "stopLocalizationService() error on writing on rpc port";
        return false;
    }
    return true;
}
