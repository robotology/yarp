/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperAmplifierControl.h"

#include <yarp/os/LogStream.h>

#include "ControlBoardWrapperLogComponent.h"


bool ControlBoardWrapperAmplifierControl::enableAmp(int j)
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

    if (p->amp) {
        return p->amp->enableAmp(off + p->base);
    }
    return false;
}


bool ControlBoardWrapperAmplifierControl::disableAmp(int j)
{
    int off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%d] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    int subIndex = device.lut[j].deviceEntry;

    bool ret = true;
    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    // Use the newer interface if available, otherwise fallback on the old one.
    if (p->iMode) {
        ret = p->iMode->setControlMode(off + p->base, VOCAB_CM_IDLE);
    } else {
        if (p->pos) {
            ret = p->amp->disableAmp(off + p->base);
        } else {
            ret = false;
        }
    }
    return ret;
}


bool ControlBoardWrapperAmplifierControl::getAmpStatus(int* st)
{
    int* status = new int[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (unsigned int d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->amp) && (ret = p->amp->getAmpStatus(status))) {
            for (int juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                st[juser] = status[jdevice];
            }
        } else {
            printError("getAmpStatus", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] status;
    return ret;
}


bool ControlBoardWrapperAmplifierControl::getAmpStatus(int j, int* v)
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
    if (p && p->amp) {
        return p->amp->getAmpStatus(off + p->base, v);
    }
    *v = 0;
    return false;
}


bool ControlBoardWrapperAmplifierControl::setMaxCurrent(int j, double v)
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

    if (p->amp) {
        return p->amp->setMaxCurrent(off + p->base, v);
    }
    return false;
}


bool ControlBoardWrapperAmplifierControl::getMaxCurrent(int j, double* v)
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
        *v = 0.0;
        return false;
    }

    if (p->amp) {
        return p->amp->getMaxCurrent(off + p->base, v);
    }
    *v = 0.0;
    return false;
}


bool ControlBoardWrapperAmplifierControl::getNominalCurrent(int m, double* val)
{
    int off = device.lut[m].offset;
    int subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        *val = 0.0;
        return false;
    }

    if (!p->amp) {
        *val = 0.0;
        return false;
    }
    return p->amp->getNominalCurrent(off + p->base, val);
}


bool ControlBoardWrapperAmplifierControl::getPeakCurrent(int m, double* val)
{
    int off = device.lut[m].offset;
    int subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        *val = 0.0;
        return false;
    }

    if (!p->amp) {
        *val = 0.0;
        return false;
    }
    return p->amp->getPeakCurrent(off + p->base, val);
}


bool ControlBoardWrapperAmplifierControl::setPeakCurrent(int m, const double val)
{
    int off = device.lut[m].offset;
    int subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (!p->amp) {
        return false;
    }
    return p->amp->setPeakCurrent(off + p->base, val);
}


bool ControlBoardWrapperAmplifierControl::setNominalCurrent(int m, const double val)
{
    int off = device.lut[m].offset;
    int subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (!p->amp) {
        return false;
    }
    return p->amp->setNominalCurrent(off + p->base, val);
}


bool ControlBoardWrapperAmplifierControl::getPWM(int m, double* val)
{
    int off = device.lut[m].offset;
    int subIndex = device.lut[m].deviceEntry;
    SubDevice* p = device.getSubdevice(subIndex);

    yCTrace(CONTROLBOARDWRAPPER) << "CBW2::getPWMlimit j" << off + p->base << " p " << (p ? "1" : "0") << " amp " << (p->amp ? "1" : "0");
    if (!p) {
        *val = 0.0;
        return false;
    }

    if (!p->amp) {
        *val = 0.0;
        return false;
    }
    return p->amp->getPWM(off + p->base, val);
}


bool ControlBoardWrapperAmplifierControl::getPWMLimit(int m, double* val)
{
    int off = device.lut[m].offset;
    int subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    yCTrace(CONTROLBOARDWRAPPER) << "CBW2::getPWMlimit j" << off + p->base << " p " << (p ? "1" : "0") << " amp " << (p->amp ? "1" : "0");

    if (!p) {
        *val = 0.0;
        return false;
    }

    if (!p->amp) {
        *val = 0.0;
        return false;
    }
    return p->amp->getPWMLimit(off + p->base, val);
}


bool ControlBoardWrapperAmplifierControl::setPWMLimit(int m, const double val)
{
    int off = device.lut[m].offset;
    int subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (!p->amp) {
        return false;
    }
    return p->amp->setPWMLimit(off + p->base, val);
}


bool ControlBoardWrapperAmplifierControl::getPowerSupplyVoltage(int m, double* val)
{
    int off = device.lut[m].offset;
    int subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        *val = 0.0;
        return false;
    }

    if (!p->amp) {
        *val = 0.0;
        return false;
    }
    return p->amp->getPowerSupplyVoltage(off + p->base, val);
}
