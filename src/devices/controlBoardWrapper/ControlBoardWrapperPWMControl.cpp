/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapperPWMControl.h"

#include "ControlBoardLogComponent.h"


bool ControlBoardWrapperPWMControl::setRefDutyCycle(int j, double v)
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

    if (p->iPWM) {
        return p->iPWM->setRefDutyCycle(static_cast<int>(off + p->base), v);
    }
    return false;
}

bool ControlBoardWrapperPWMControl::setRefDutyCycles(const double* v)
{
    bool ret = true;

    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->iPWM) {
            ret = ret && p->iPWM->setRefDutyCycle(static_cast<int>(off + p->base), v[l]);
        } else {
            ret = false;
        }
    }
    return ret;
}

bool ControlBoardWrapperPWMControl::getRefDutyCycle(int j, double* v)
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

    if (p->iPWM) {
        return p->iPWM->getRefDutyCycle(static_cast<int>(off + p->base), v);
    }
    return false;
}

bool ControlBoardWrapperPWMControl::getRefDutyCycles(double* v)
{
    auto* references = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iPWM) && (ret = p->iPWM->getRefDutyCycles(references))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
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

    if (p->iPWM) {
        return p->iPWM->getDutyCycle(static_cast<int>(off + p->base), v);
    }
    return false;
}

bool ControlBoardWrapperPWMControl::getDutyCycles(double* v)
{
    auto* dutyCicles = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iPWM) && (ret = p->iPWM->getDutyCycles(dutyCicles))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
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
