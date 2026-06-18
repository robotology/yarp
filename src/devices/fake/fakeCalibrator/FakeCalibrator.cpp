/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>

#include "fakeCalibrator.h"

#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;


FakeCalibrator::FakeCalibrator()
{
}

FakeCalibrator::~FakeCalibrator()
{
    yTrace();
    close();
}

bool FakeCalibrator::open(yarp::os::Searchable& config)
{
    return true;
}

bool FakeCalibrator::close ()
{
    yTrace();
    return true;
}

ReturnValue FakeCalibrator::calibrate(DeviceDriver *device)
{
    yTrace();

    if (device==nullptr)
    {
        yError() << ": invalid device driver";
        return ReturnValue_error_not_ready;
    }

    yarp::dev::PolyDriver *p = dynamic_cast<yarp::dev::PolyDriver *>(device);
    m_dev2calibrate = p;
    if (p!=nullptr)
    {
        p->view(m_iControlCalibration);
        p->view(m_iEncoders);
    }
    else
    {
        yWarning() << ": using parametricCalibrator on an old iCubInterface system. Upgrade to robotInterface is recommended."; 
        device->view(m_iControlCalibration);
        device->view(m_iEncoders);
    }

    if (!(m_iControlCalibration && m_iEncoders))
    {
        yError() << ": interface not found";
        return ReturnValue_error_method_failed;
    }

    return ReturnValue_ok;
}

ReturnValue FakeCalibrator::park(DeviceDriver *dd, bool wait)
{
   return ReturnValue_ok;
}

ReturnValue FakeCalibrator::quitCalibrate()
{
    return ReturnValue_ok;
}

ReturnValue FakeCalibrator::quitPark()
{
    return ReturnValue_ok;
}

yarp::dev::IRemoteCalibrator* FakeCalibrator::getCalibratorDevice()
{
    return this;
}

ReturnValue FakeCalibrator::calibrateSingleJoint(int j)
{
    if (m_iControlCalibration == nullptr)
        return ReturnValue_error_not_ready;

    auto b = m_iControlCalibration->setCalibrationParameters(j, m_cparams);
    return b;
}

ReturnValue FakeCalibrator::calibrateWholePart()
{
    yTrace();
    return calibrateSingleJoint(0);
}

ReturnValue FakeCalibrator::homingSingleJoint(int j)
{
    yTrace();
    return ReturnValue_ok;
}

ReturnValue FakeCalibrator::homingWholePart()
{
    yTrace();
    return homingSingleJoint(0);
}

ReturnValue FakeCalibrator::parkSingleJoint(int j, bool _wait)
{
    yTrace();
    return ReturnValue_ok;
}

// called from motorgui or remote devices
ReturnValue FakeCalibrator::parkWholePart()
{
    yTrace();
    return park(m_dev2calibrate);
}
