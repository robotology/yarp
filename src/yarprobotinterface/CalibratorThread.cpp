/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "CalibratorThread.h"

#include <debugStream/Debug.h>

#include <yarp/os/Semaphore.h>
#include <yarp/dev/CalibratorInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>

class RobotInterface::CalibratorThread::Private
{
public:
    Private(CalibratorThread *parent) : parent(parent) {}

    bool init()
    {
        switch (action)
        {
        case ActionCalibrate:
            if (!calibrator) {
                yError() << "Skipping calibration, calibrator not set";
                return false;
            }

            if (!target) {
                yError() << "Skipping calibration, target not set";
                return false;
            }

            if (!threadList && !threadListSemaphore) {
                yError() << "Skipping calibration, thread list and/or semaphore not set";
                return false;
            }
            break;

        case ActionPark:
            if (!calibrator) {
                yError() << "Skipping park, calibrator not set";
                return false;
            }

            if (!target) {
                yError() << "Skipping park, target not set";
                return false;
            }

            if (!threadList && !threadListSemaphore) {
                yError() << "Skipping park, thread list and/or semaphore not set";
                return false;
            }
            break;
        }

        return true;
    }

    void run()
    {
        threadListSemaphore->wait();
        threadList->push_back(parent);
        threadListSemaphore->post();

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

        threadListSemaphore->wait();
        threadList->remove(parent);
        threadListSemaphore->post();
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
    yarp::dev::DeviceDriver *target;
    RobotInterface::CalibratorThread::Action action;
    RobotInterface::ThreadList *threadList;
    yarp::os::Semaphore *threadListSemaphore;
    std::string calibratorName;
    std::string targetName;
}; // class RobotInterface::CalibratorThread::Private



RobotInterface::CalibratorThread::CalibratorThread(yarp::dev::ICalibrator *calibrator,
                                                   yarp::dev::DeviceDriver *target,
                                                   RobotInterface::CalibratorThread::Action action,
                                                   RobotInterface::ThreadList *threadList,
                                                   yarp::os::Semaphore *threadListSemaphore,
                                                   const std::string &calibratorName,
                                                   const std::string &targetName) :
        mPriv(new Private(this))
{
    mPriv->calibrator = calibrator;
    mPriv->target = target;
    mPriv->action = action;
    mPriv->threadList = threadList;
    mPriv->threadListSemaphore = threadListSemaphore;
    mPriv->calibratorName = calibratorName;
    mPriv->targetName = targetName;
}

RobotInterface::CalibratorThread::~CalibratorThread()
{
    delete mPriv;
}


bool RobotInterface::CalibratorThread::threadInit()
{
    return mPriv->init();
}

void RobotInterface::CalibratorThread::run()
{
    mPriv->run();
}

void RobotInterface::CalibratorThread::onStop()
{
    mPriv->stop();
}