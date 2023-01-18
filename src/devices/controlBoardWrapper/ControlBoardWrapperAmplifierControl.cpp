/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapperAmplifierControl.h"

#include <yarp/os/LogStream.h>

#include "ControlBoardLogComponent.h"


bool ControlBoardWrapperAmplifierControl::enableAmp(int j)
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

    if (p->amp) {
        return p->amp->enableAmp(static_cast<int>(off + p->base));
    }
    return false;
}


bool ControlBoardWrapperAmplifierControl::disableAmp(int j)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARD, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    bool ret = true;
    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    // Use the newer interface if available, otherwise fallback on the old one.
    if (p->iMode) {
        ret = p->iMode->setControlMode(static_cast<int>(off + p->base), VOCAB_CM_IDLE);
    } else {
        if (p->pos) {
            ret = p->amp->disableAmp(static_cast<int>(off + p->base));
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
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->amp) && (ret = p->amp->getAmpStatus(status))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
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
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARD, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (p && p->amp) {
        return p->amp->getAmpStatus(static_cast<int>(off + p->base), v);
    }
    *v = 0;
    return false;
}


bool ControlBoardWrapperAmplifierControl::setMaxCurrent(int j, double v)
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

    if (p->amp) {
        return p->amp->setMaxCurrent(static_cast<int>(off + p->base), v);
    }
    return false;
}


bool ControlBoardWrapperAmplifierControl::getMaxCurrent(int j, double* v)
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
        *v = 0.0;
        return false;
    }

    if (p->amp) {
        return p->amp->getMaxCurrent(static_cast<int>(off + p->base), v);
    }
    *v = 0.0;
    return false;
}


bool ControlBoardWrapperAmplifierControl::getNominalCurrent(int m, double* val)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        *val = 0.0;
        return false;
    }

    if (!p->amp) {
        *val = 0.0;
        return false;
    }
    return p->amp->getNominalCurrent(static_cast<int>(off + p->base), val);
}


bool ControlBoardWrapperAmplifierControl::getPeakCurrent(int m, double* val)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        *val = 0.0;
        return false;
    }

    if (!p->amp) {
        *val = 0.0;
        return false;
    }
    return p->amp->getPeakCurrent(static_cast<int>(off + p->base), val);
}


bool ControlBoardWrapperAmplifierControl::setPeakCurrent(int m, const double val)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (!p->amp) {
        return false;
    }
    return p->amp->setPeakCurrent(static_cast<int>(off + p->base), val);
}


bool ControlBoardWrapperAmplifierControl::setNominalCurrent(int m, const double val)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (!p->amp) {
        return false;
    }
    return p->amp->setNominalCurrent(static_cast<int>(off + p->base), val);
}


bool ControlBoardWrapperAmplifierControl::getPWM(int m, double* val)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;
    SubDevice* p = device.getSubdevice(subIndex);

    yCTrace(CONTROLBOARD) << "CBW2::getPWMlimit j" << static_cast<int>(off + p->base) << " p " << (p ? "1" : "0") << " amp " << (p->amp ? "1" : "0");
    if (!p) {
        *val = 0.0;
        return false;
    }

    if (!p->amp) {
        *val = 0.0;
        return false;
    }
    return p->amp->getPWM(static_cast<int>(off + p->base), val);
}


bool ControlBoardWrapperAmplifierControl::getPWMLimit(int m, double* val)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    yCTrace(CONTROLBOARD) << "CBW2::getPWMlimit j" << static_cast<int>(off + p->base) << " p " << (p ? "1" : "0") << " amp " << (p->amp ? "1" : "0");

    if (!p) {
        *val = 0.0;
        return false;
    }

    if (!p->amp) {
        *val = 0.0;
        return false;
    }
    return p->amp->getPWMLimit(static_cast<int>(off + p->base), val);
}


bool ControlBoardWrapperAmplifierControl::setPWMLimit(int m, const double val)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (!p->amp) {
        return false;
    }
    return p->amp->setPWMLimit(static_cast<int>(off + p->base), val);
}


bool ControlBoardWrapperAmplifierControl::getPowerSupplyVoltage(int m, double* val)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        *val = 0.0;
        return false;
    }

    if (!p->amp) {
        *val = 0.0;
        return false;
    }
    return p->amp->getPowerSupplyVoltage(static_cast<int>(off + p->base), val);
}
