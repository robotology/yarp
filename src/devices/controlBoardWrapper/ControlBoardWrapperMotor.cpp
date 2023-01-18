/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapperMotor.h"

#include "ControlBoardLogComponent.h"

bool ControlBoardWrapperMotor::getTemperature(int m, double* val)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->imotor) {
        return p->imotor->getTemperature(static_cast<int>(off + p->base), val);
    }
    *val = 0.0;
    return false;
}

bool ControlBoardWrapperMotor::getTemperatures(double* vals)
{
    auto* temps = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->imotor) && (ret = p->imotor->getTemperatures(temps))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                vals[juser] = temps[jdevice];
            }
        } else {
            printError("getTemperatures", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] temps;
    return ret;
}

bool ControlBoardWrapperMotor::getTemperatureLimit(int m, double* val)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->imotor) {
        return p->imotor->getTemperatureLimit(static_cast<int>(off + p->base), val);
    }
    *val = 0.0;
    return false;
}

bool ControlBoardWrapperMotor::setTemperatureLimit(int m, const double val)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->imotor) {
        return p->imotor->setTemperatureLimit(static_cast<int>(off + p->base), val);
    }
    return false;
}

bool ControlBoardWrapperMotor::getGearboxRatio(int m, double* val)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->imotor) {
        return p->imotor->getGearboxRatio(static_cast<int>(off + p->base), val);
    }
    *val = 0.0;
    return false;
}

bool ControlBoardWrapperMotor::setGearboxRatio(int m, const double val)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->imotor) {
        return p->imotor->setGearboxRatio(static_cast<int>(off + p->base), val);
    }
    return false;
}
