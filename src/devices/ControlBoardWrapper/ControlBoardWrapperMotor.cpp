/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperMotor.h"

#include "ControlBoardWrapperLogComponent.h"

bool ControlBoardWrapperMotor::getTemperature(int m, double* val)
{
    int off = device.lut[m].offset;
    int subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->imotor) {
        return p->imotor->getTemperature(off + p->base, val);
    }
    *val = 0.0;
    return false;
}

bool ControlBoardWrapperMotor::getTemperatures(double* vals)
{
    auto* temps = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (unsigned int d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->imotor) && (ret = p->imotor->getTemperatures(temps))) {
            for (int juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
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
    int subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->imotor) {
        return p->imotor->getTemperatureLimit(off + p->base, val);
    }
    *val = 0.0;
    return false;
}

bool ControlBoardWrapperMotor::setTemperatureLimit(int m, const double val)
{
    int off = device.lut[m].offset;
    int subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->imotor) {
        return p->imotor->setTemperatureLimit(off + p->base, val);
    }
    return false;
}

bool ControlBoardWrapperMotor::getGearboxRatio(int m, double* val)
{
    int off = device.lut[m].offset;
    int subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->imotor) {
        return p->imotor->getGearboxRatio(off + p->base, val);
    }
    *val = 0.0;
    return false;
}

bool ControlBoardWrapperMotor::setGearboxRatio(int m, const double val)
{
    int off = device.lut[m].offset;
    int subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->imotor) {
        return p->imotor->setGearboxRatio(off + p->base, val);
    }
    return false;
}
