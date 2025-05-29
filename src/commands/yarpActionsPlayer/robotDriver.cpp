/*
 * SPDX-FileCopyrightText: 2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

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

    if (verbose)
    {
        yDebug() << "driver options:\n" << drvOptions_ll.toString().c_str();
    }

    return ret;
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

    //set the initial reference speeds
    std::vector<double> speeds;
    for (int i = 0; i < n_joints; i++)
    {
        speeds.push_back(20.0);
    }
    ok &= ipos_ll->setRefSpeeds(speeds.data());

    return ok;
}

robotDriver::~robotDriver()
{
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
    return ipos_ll->positionMove(j, v);
}

size_t robotDriver::getNJoints()
{
    return n_joints;
}
