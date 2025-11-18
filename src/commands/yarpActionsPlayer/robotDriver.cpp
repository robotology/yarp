/*
 * SPDX-FileCopyrightText: 2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cmath>
#include <iomanip>

#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Vector.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Thread.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

#include "robotDriver.h"

// ******************** ROBOT DRIVER CLASS
robotDriver::robotDriver()
{
    drvOptions_ll.clear();
}

bool robotDriver::configure(const Property &copt)
{
    bool ret=true;
    Property &options=const_cast<Property &> (copt);

    drvOptions_ll.put("device","remotecontrolboardremapper");

    yDebug() << "remoteControlBoards will use the following configuration:";
    yDebug() << "set of joints:" << options.find("axesNames").toString();
    yDebug() << "set of controlboards:" << options.find("remoteControlBoards").toString();
    drvOptions_ll.put("axesNames", options.find("axesNames"));
    drvOptions_ll.put("remoteControlBoards",options.find("remoteControlBoards"));
    drvOptions_ll.put("localPortPrefix", options.find("localPortPrefix"));

    yarp::os::Property& pcb =drvOptions_ll.addGroup("REMOTE_CONTROLBOARD_OPTIONS");
    pcb.put("carrier", "fast_tcp");

    if (m_verbose)
    {
        yDebug() << "driver options:\n" << drvOptions_ll.toString().c_str();
    }

    return ret;
}

bool robotDriver::setTrajectoryTime(double t)
{
    for (size_t i=0; i<n_joints; i++)
    m_trajectoryTime[i] = t;
    return true;
}

bool robotDriver::setTrajectoryTime(int j, double t)
{
    m_trajectoryTime[j] = t;
    return true;
}

bool robotDriver::init()
{
    drv_ll.open(drvOptions_ll);
    bool ok = true;

    if (drv_ll.isValid())
    {
        ok = drv_ll.view(ipos_ll) && drv_ll.view(iposdir_ll) && drv_ll.view(ienc_ll) &&
             drv_ll.view(ipid_ll) && drv_ll.view(imotenc_ll) && drv_ll.view(icmd_ll);
    }
    else
    {
        ok = false;
    }

    if (!ok)
    {
        return false;
    }

    //get the number of the joints
    ok &= ienc_ll->getAxes(&n_joints);

    //set the trajectory duration for the first movement
    m_trajectoryTime.resize(n_joints,4.0);

    //set the initial reference speeds to 20
    for (int i = 0; i < n_joints; i++)
    {
        m_stored_speed.push_back(20.0);
    }

    ok &= loadRefVelocities();

    return ok;
}

robotDriver::~robotDriver()
{
}

bool robotDriver::getControlMode(const int j, int& mode)
{
    if (!icmd_ll) return false;
    return icmd_ll->getControlMode(j, &mode);
}

bool robotDriver::setControlMode(const int j, const int mode)
{
    if (!icmd_ll) return false;
    return icmd_ll->setControlMode(j, mode);
}

bool robotDriver::setPosition(int j, double ref)
{
    if (!iposdir_ll) return false;
    return iposdir_ll->setPosition(j, ref);
}

bool robotDriver::getEncoder(int j, double *v)
{
    if (!ienc_ll) return false;
    return ienc_ll->getEncoder(j, v);
}

bool robotDriver::positionMove(int j, double v)
{
    if (!ipos_ll) return false;
    bool ret = true;

    // set ref speed to each joint so that the motion is completed in
    // the requested amount of time
    if (1)
    {
        double enc = 0;
        ret &= ienc_ll->getEncoder(j, &enc);
        double diff = fabs(v - enc);
        double speed = diff/m_trajectoryTime[j];
        ipos_ll->setTrajSpeed(j, speed);
    }

    ret &= ipos_ll->positionMove(j, v);

    return ret;
}

size_t robotDriver::getNJoints()
{
    return n_joints;
}

std::string vectorToString(const std::vector<double>& vec)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "(";
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << vec[i];
        if (i + 1 != vec.size()) oss << " ";
    }
    oss << ")";
    return oss.str();
}

bool robotDriver::storeRefVelocities()
{
    bool ok;
    ok = ipos_ll->getTrajSpeeds(m_stored_speed.data());
    yDebug() << "Storing speeds" << vectorToString(m_stored_speed);
    return ok;
}

bool robotDriver::loadRefVelocities()
{
    bool ok;
    yDebug() << "Loading speeds"  << vectorToString(m_stored_speed);
    ok = ipos_ll->setTrajSpeeds(m_stored_speed.data());
    return ok;
}
