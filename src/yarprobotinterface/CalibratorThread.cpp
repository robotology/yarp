/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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

#include "CalibratorThread.h"

#include <yarp/os/LogStream.h>

#include <yarp/os/Log.h>
#include <yarp/dev/CalibratorInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>

class RobotInterface::CalibratorThread::Private
{
public:
    Private(CalibratorThread *parent) :
            parent(parent),
            calibrator(nullptr),
            target(nullptr),
            action(RobotInterface::CalibratorThread::Action::ActionCalibrate)
    {
    }

    void run()
    {
        switch (action)
        {
        case ActionCalibrate:
            yDebug() << calibratorName << "starting calibration of device" << targetName;
            calibrator->calibrate(target);
            yDebug() << calibratorName << "finished calibration of device" << targetName;
            break;
        case ActionPark:
            yDebug() << calibratorName << "starting park device" << targetName;
            calibrator->park(target);
            yDebug() << calibratorName << "finished park device" << targetName;
            break;
        }
    }

    void stop()
    {
        switch (action)
        {
        case ActionCalibrate:
            yDebug() << calibratorName << "killing calibration of device" << targetName;
            calibrator->quitCalibrate();
            break;
        case ActionPark:
            yDebug() << calibratorName << "killing park of device" << targetName;
            calibrator->quitPark();
            break;
        }
    }

    RobotInterface::CalibratorThread * const parent;

    yarp::dev::ICalibrator *calibrator;
    std::string calibratorName;
    yarp::dev::DeviceDriver *target;
    std::string targetName;
    RobotInterface::CalibratorThread::Action action;
}; // class RobotInterface::CalibratorThread::Private



RobotInterface::CalibratorThread::CalibratorThread(yarp::dev::ICalibrator *calibrator,
                                                   const std::string &calibratorName,
                                                   yarp::dev::DeviceDriver *target,
                                                   const std::string &targetName,
                                                   RobotInterface::CalibratorThread::Action action) :
        mPriv(new Private(this))
{
    yAssert(calibrator);
    yAssert(target);
    yAssert(action == ActionCalibrate || action == ActionPark);

    mPriv->calibrator = calibrator;
    mPriv->calibratorName = calibratorName;
    mPriv->target = target;
    mPriv->targetName = targetName;
    mPriv->action = action;
}

RobotInterface::CalibratorThread::~CalibratorThread()
{
    delete mPriv;
}

void RobotInterface::CalibratorThread::run()
{
    mPriv->run();
}

void RobotInterface::CalibratorThread::onStop()
{
    mPriv->stop();
}
