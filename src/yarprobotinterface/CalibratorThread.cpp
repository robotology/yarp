/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "CalibratorThread.h"

#include <yarp/os/LogStream.h>

#include <yarp/os/Log.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/CalibratorInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>

class RobotInterface::CalibratorThread::Private
{
public:
    Private(CalibratorThread *parent) : parent(parent) {}

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
