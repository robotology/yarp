/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Navigation2D_nwc_yarp.h"
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <cmath>

/*! \file Navigation2D_nwc_yarp_iNav2DTarget.cpp */

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(NAVIGATION2D_NWC, "yarp.device.navigation2D_nwc_yarp")
}

//------------------------------------------------------------------------------------------------------------------------------

bool Navigation2D_nwc_yarp::gotoTargetByAbsoluteLocation(Map2DLocation loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GOTOABS);
    b.addString(loc.map_id);
    b.addFloat64(loc.x);
    b.addFloat64(loc.y);
    b.addFloat64(loc.theta);
    bool ret = m_rpc_port_to_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2D_NWC) << "gotoTargetByAbsoluteLocation() received error from navigation server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2D_NWC) << "gotoTargetByAbsoluteLocation() error on writing on rpc port";
        return false;
    }

    return true;
}

bool Navigation2D_nwc_yarp::gotoTargetByRelativeLocation(double x, double y)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GOTOREL);
    b.addFloat64(x);
    b.addFloat64(y);

    bool ret = m_rpc_port_to_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2D_NWC) << "gotoTargetByRelativeLocation() received error from navigation server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2D_NWC) << "gotoTargetByRelativeLocation() error on writing on rpc port";
        return false;
    }

    return true;
}

bool Navigation2D_nwc_yarp::gotoTargetByRelativeLocation(double x, double y, double theta)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GOTOREL);
    b.addFloat64(x);
    b.addFloat64(y);
    b.addFloat64(theta);

    bool ret = m_rpc_port_to_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2D_NWC) << "gotoTargetByRelativeLocation() received error from navigation server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2D_NWC) << "gotoTargetByRelativeLocation() error on writing on rpc port";
        return false;
    }

    return true;
}

bool Navigation2D_nwc_yarp::getAbsoluteLocationOfCurrentTarget(Map2DLocation &loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_ABS_TARGET);

    bool ret = m_rpc_port_to_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK || resp.size() != 5)
        {
            yCError(NAVIGATION2D_NWC) << "getAbsoluteLocationOfCurrentTarget() received error from navigation server";
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
        yCError(NAVIGATION2D_NWC) << "getAbsoluteLocationOfCurrentTarget() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2D_nwc_yarp::getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_REL_TARGET);

    bool ret = m_rpc_port_to_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK || resp.size()!=4)
        {
            yCError(NAVIGATION2D_NWC) << "getRelativeLocationOfCurrentTarget() received error from navigation server";
            return false;
        }
        else
        {
            x = resp.get(1).asFloat64();
            y = resp.get(2).asFloat64();
            theta = resp.get(3).asFloat64();
            return true;
        }
    }
    else
    {
        yCError(NAVIGATION2D_NWC) << "getRelativeLocationOfCurrentTarget() error on writing on rpc port";
        return false;
    }
    return true;
}
