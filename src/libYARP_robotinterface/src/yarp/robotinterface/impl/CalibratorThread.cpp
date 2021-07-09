/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <yarp/dev/CalibratorInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>

#include <yarp/robotinterface/impl/CalibratorThread.h>

class yarp::robotinterface::impl::CalibratorThread::Private
{
public:
    Private(CalibratorThread* parent) :
            parent(parent),
            calibrator(nullptr),
            target(nullptr),
            action(yarp::robotinterface::impl::CalibratorThread::Action::ActionCalibrate)
    {
    }

    void run()
    {
        switch (action) {
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
        switch (action) {
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

    yarp::robotinterface::impl::CalibratorThread* const parent;

    yarp::dev::ICalibrator* calibrator;
    std::string calibratorName;
    yarp::dev::DeviceDriver* target;
    std::string targetName;
    yarp::robotinterface::impl::CalibratorThread::Action action;
}; // class yarp::robotinterface::impl::CalibratorThread::Private


yarp::robotinterface::impl::CalibratorThread::CalibratorThread(yarp::dev::ICalibrator* calibrator,
                                                               const std::string& calibratorName,
                                                               yarp::dev::DeviceDriver* target,
                                                               const std::string& targetName,
                                                               yarp::robotinterface::impl::CalibratorThread::Action action) :
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

yarp::robotinterface::impl::CalibratorThread::~CalibratorThread()
{
    delete mPriv;
}

void yarp::robotinterface::impl::CalibratorThread::run()
{
    mPriv->run();
}

void yarp::robotinterface::impl::CalibratorThread::onStop()
{
    mPriv->stop();
}
