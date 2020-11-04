/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperEncodersTimed.h"

#include "ControlBoardWrapperLogComponent.h"

bool ControlBoardWrapperEncodersTimed::resetEncoder(int j)
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

    if (p->iJntEnc) {
        return p->iJntEnc->resetEncoder(off + p->base);
    }
    return false;
}


bool ControlBoardWrapperEncodersTimed::resetEncoders()
{
    bool ret = true;

    for (int l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        int subIndex = device.lut[l].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->iJntEnc) {
            ret = ret && p->iJntEnc->resetEncoder(off + p->base);
        } else {
            ret = false;
        }
    }
    return ret;
}


bool ControlBoardWrapperEncodersTimed::setEncoder(int j, double val)
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

    if (p->iJntEnc) {
        return p->iJntEnc->setEncoder(off + p->base, val);
    }
    return false;
}


bool ControlBoardWrapperEncodersTimed::setEncoders(const double* vals)
{
    bool ret = true;

    for (int l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        int subIndex = device.lut[l].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->iJntEnc) {
            ret = ret && p->iJntEnc->setEncoder(off + p->base, vals[l]);
        } else {
            ret = false;
        }
    }
    return ret;
}


bool ControlBoardWrapperEncodersTimed::getEncoder(int j, double* v)
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

    if (p->iJntEnc) {
        return p->iJntEnc->getEncoder(off + p->base, v);
    }
    *v = 0.0;
    return false;
}


bool ControlBoardWrapperEncodersTimed::getEncoders(double* encs)
{
    auto* encValues = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (unsigned int d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iJntEnc) && (ret = p->iJntEnc->getEncoders(encValues))) {
            for (int juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                encs[juser] = encValues[jdevice];
            }
        } else {
            printError("getEncoders", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] encValues;
    return ret;
}


bool ControlBoardWrapperEncodersTimed::getEncodersTimed(double* encs, double* t)
{
    auto* encValues = new double[device.maxNumOfJointsInDevices];
    auto* tValues = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (unsigned int d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iJntEnc) && (ret = p->iJntEnc->getEncodersTimed(encValues, tValues))) {
            for (int juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                encs[juser] = encValues[jdevice];
                t[juser] = tValues[jdevice];
            }
        } else {
            printError("getEncodersTimed", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] encValues;
    delete[] tValues;
    return ret;
}


bool ControlBoardWrapperEncodersTimed::getEncoderTimed(int j, double* v, double* t)
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

    if (p->iJntEnc) {
        return p->iJntEnc->getEncoderTimed(off + p->base, v, t);
    }
    *v = 0.0;
    return false;
}


bool ControlBoardWrapperEncodersTimed::getEncoderSpeed(int j, double* sp)
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

    if (p->iJntEnc) {
        return p->iJntEnc->getEncoderSpeed(off + p->base, sp);
    }
    *sp = 0.0;
    return false;
}


bool ControlBoardWrapperEncodersTimed::getEncoderSpeeds(double* spds)
{
    auto* sValues = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (unsigned int d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iJntEnc) && (ret = p->iJntEnc->getEncoderSpeeds(sValues))) {
            for (int juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                spds[juser] = sValues[jdevice];
            }
        } else {
            printError("getEncoderSpeeds", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] sValues;
    return ret;
}


bool ControlBoardWrapperEncodersTimed::getEncoderAcceleration(int j, double* acc)
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

    if (p->iJntEnc) {
        return p->iJntEnc->getEncoderAcceleration(off + p->base, acc);
    }
    *acc = 0.0;
    return false;
}


bool ControlBoardWrapperEncodersTimed::getEncoderAccelerations(double* accs)
{
    auto* aValues = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (unsigned int d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iJntEnc) && (ret = p->iJntEnc->getEncoderAccelerations(aValues))) {
            for (int juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                accs[juser] = aValues[jdevice];
            }
        } else {
            printError("getEncoderAccelerations", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] aValues;
    return ret;
}
