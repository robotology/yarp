/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperPWMControl.h"

#include "ControlBoardWrapperLogComponent.h"


bool ControlBoardWrapperPWMControl::setRefDutyCycle(int j, double v)
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

    if (p->iPWM) {
        return p->iPWM->setRefDutyCycle(off + p->base, v);
    }
    return false;
}

bool ControlBoardWrapperPWMControl::setRefDutyCycles(const double* v)
{
    bool ret = true;

    for (int l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        int subIndex = device.lut[l].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->iPWM) {
            ret = ret && p->iPWM->setRefDutyCycle(off + p->base, v[l]);
        } else {
            ret = false;
        }
    }
    return ret;
}

bool ControlBoardWrapperPWMControl::getRefDutyCycle(int j, double* v)
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

    if (p->iPWM) {
        return p->iPWM->getRefDutyCycle(off + p->base, v);
    }
    return false;
}

bool ControlBoardWrapperPWMControl::getRefDutyCycles(double* v)
{
    auto* references = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (unsigned int d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iPWM) && (ret = p->iPWM->getRefDutyCycles(references))) {
            for (int juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                v[juser] = references[jdevice];
            }
        } else {
            printError("getRefDutyCycles", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] references;
    return ret;
}

bool ControlBoardWrapperPWMControl::getDutyCycle(int j, double* v)
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

    if (p->iPWM) {
        return p->iPWM->getDutyCycle(off + p->base, v);
    }
    return false;
}

bool ControlBoardWrapperPWMControl::getDutyCycles(double* v)
{
    auto* dutyCicles = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (unsigned int d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iPWM) && (ret = p->iPWM->getDutyCycles(dutyCicles))) {
            for (int juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                v[juser] = dutyCicles[jdevice];
            }
        } else {
            printError("getDutyCycles", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] dutyCicles;
    return ret;
}
