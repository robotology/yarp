/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapperControlCalibration.h"

#include "ControlBoardLogComponent.h"

using yarp::dev::CalibrationParameters;

bool ControlBoardWrapperControlCalibration::calibrateAxisWithParams(int j, unsigned int ui, double v1, double v2, double v3)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARD, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (p && p->calib) {
        return p->calib->calibrateAxisWithParams(static_cast<int>(off + p->base), ui, v1, v2, v3);
    }
    return false;
}


bool ControlBoardWrapperControlCalibration::setCalibrationParameters(int j, const CalibrationParameters& params)
{
    int off = device.lut[j].offset;
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (p && p->calib) {
        return p->calib->setCalibrationParameters(static_cast<int>(off + p->base), params);
    }
    return false;
}


bool ControlBoardWrapperControlCalibration::calibrationDone(int j)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARD, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->calib) {
        return p->calib->calibrationDone(static_cast<int>(off + p->base));
    }
    return false;
}


bool ControlBoardWrapperControlCalibration::abortPark()
{
    yCError(CONTROLBOARD, "Calling abortPark -- not implemented");
    return false;
}


bool ControlBoardWrapperControlCalibration::abortCalibration()
{
    yCError(CONTROLBOARD, "Calling abortCalibration -- not implemented");
    return false;
}
