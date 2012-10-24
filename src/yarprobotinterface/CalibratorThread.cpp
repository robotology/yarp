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

        return true;
    }

    void run()
    {
        threadListSemaphore->wait();
        threadList->push_back(parent);
        threadListSemaphore->post();

        calibrator->calibrate(target);

        threadListSemaphore->wait();
        threadList->remove(parent);
        threadListSemaphore->post();
    }

    void stop()
    {
        calibrator->quitCalibrate();
    }

    RobotInterface::CalibratorThread * const parent;

    yarp::dev::ICalibrator *calibrator;
    yarp::dev::DeviceDriver *target;
    RobotInterface::ThreadList *threadList;
    yarp::os::Semaphore *threadListSemaphore;
}; // class RobotInterface::CalibratorThread::Private



RobotInterface::CalibratorThread::CalibratorThread(yarp::dev::ICalibrator *calibrator,
                                                   yarp::dev::DeviceDriver *target,
                                                   RobotInterface::ThreadList *threadList,
                                                   yarp::os::Semaphore *threadListSemaphore) :
        mPriv(new Private(this))
{
    mPriv->calibrator = calibrator;
    mPriv->target = target;
    mPriv->threadList = threadList;
    mPriv->threadListSemaphore = threadListSemaphore;
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
    yDebug() << "Starting calibration";
    mPriv->run();
    yDebug() << "Calibration done";
}

void RobotInterface::CalibratorThread::onStop()
{
    yDebug() << "Killing calibration";
    mPriv->stop();
}