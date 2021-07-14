/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ImplementControlCalibration.h>
#include <yarp/os/Log.h>

#include <cstdio>

using namespace yarp::dev;

IControlCalibration::IControlCalibration()
{
    calibrator=nullptr;
}

bool IControlCalibration::setCalibrator(ICalibrator *c)
{
    if (c!=nullptr)
    {
        calibrator=c;
        return true;
    }

    return false;
}

bool IControlCalibration::calibrateRobot()
{
    bool ret = false;
    if (calibrator!=nullptr)
    {
        yDebug("Going to call calibrator\n");
        ret=calibrator->calibrate(dynamic_cast<DeviceDriver *>(this));
    } else {
        yWarning("Warning called calibrate but no calibrator was set\n");
    }

    return ret;
}

bool IControlCalibration::abortCalibration()
{
    bool ret=false;
    if (calibrator != nullptr) {
        ret = calibrator->quitCalibrate();
    }
    return ret;
}

bool IControlCalibration::abortPark()
{
    bool ret=false;
    if (calibrator != nullptr) {
        ret = calibrator->quitPark();
    }
    return ret;
}

bool IControlCalibration::park(bool wait)
{
    bool ret=false;
    if (calibrator!=nullptr)
    {
        yDebug("Going to call calibrator\n");
        ret=calibrator->park(dynamic_cast<DeviceDriver *>(this), wait);
    } else {
        yWarning("Warning called park but no calibrator was set\n");
    }

    return ret;
}

////////////////////////
// ControlCalibration Interface Implementation
ImplementControlCalibration::ImplementControlCalibration(yarp::dev::IControlCalibrationRaw *y)
{
    iCalibrate = dynamic_cast<IControlCalibrationRaw *> (y);
    helper = nullptr;
    temp = nullptr;
}

ImplementControlCalibration::~ImplementControlCalibration()
{
    uninitialize();
}

bool ImplementControlCalibration::initialize(int size, const int *amap, const double *enc, const double *zos)
{
    if (helper != nullptr) {
        return false;
    }

    helper = (void *)(new ControlBoardHelper(size, amap, enc, zos));
    yAssert(helper != nullptr);
    temp = new double[size];
    yAssert(temp != nullptr);
    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementControlCalibration::uninitialize()
{
    if (helper != nullptr)
    {
        delete castToMapper(helper);
        helper = nullptr;
    }

    checkAndDestroy(temp);

    return true;
}

bool ImplementControlCalibration::calibrationDone(int j)
{
    int k = castToMapper(helper)->toHw(j);

    return iCalibrate->calibrationDoneRaw(k);
}

bool ImplementControlCalibration::calibrateAxisWithParams(int axis, unsigned int type, double p1, double p2, double p3)
{
    int k = castToMapper(helper)->toHw(axis);

    return iCalibrate->calibrateAxisWithParamsRaw(k, type, p1, p2, p3);
}

bool ImplementControlCalibration::setCalibrationParameters(int axis, const CalibrationParameters& params)
{
    int k = castToMapper(helper)->toHw(axis);

    return iCalibrate->setCalibrationParametersRaw(k, params);
}
