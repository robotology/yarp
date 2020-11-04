/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperControlCalibration.h"

#include "ControlBoardWrapperLogComponent.h"

using yarp::dev::CalibrationParameters;

bool ControlBoardWrapperControlCalibration::calibrateAxisWithParams(int j, unsigned int ui, double v1, double v2, double v3)
{
    int off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%d] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    int subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (p && p->calib) {
        return p->calib->calibrateAxisWithParams(off + p->base, ui, v1, v2, v3);
    }
    return false;
}


bool ControlBoardWrapperControlCalibration::setCalibrationParameters(int j, const CalibrationParameters& params)
{
    int off = device.lut[j].offset;
    int subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (p && p->calib) {
        return p->calib->setCalibrationParameters(off + p->base, params);
    }
    return false;
}


bool ControlBoardWrapperControlCalibration::calibrationDone(int j)
{
    int off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%d] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    int subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->calib) {
        return p->calib->calibrationDone(off + p->base);
    }
    return false;
}


bool ControlBoardWrapperControlCalibration::abortPark()
{
    yCError(CONTROLBOARDWRAPPER, "Calling abortPark -- not implemented");
    return false;
}


bool ControlBoardWrapperControlCalibration::abortCalibration()
{
    yCError(CONTROLBOARDWRAPPER, "Calling abortCalibration -- not implemented");
    return false;
}
