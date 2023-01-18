/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapperMotorEncoders.h"

#include "ControlBoardLogComponent.h"

bool ControlBoardWrapperMotorEncoders::resetMotorEncoder(int m)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iMotEnc) {
        return p->iMotEnc->resetMotorEncoder(static_cast<int>(off + p->base));
    }
    return false;
}


bool ControlBoardWrapperMotorEncoders::resetMotorEncoders()
{
    bool ret = true;

    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->iMotEnc) {
            ret = ret && p->iMotEnc->resetMotorEncoder(static_cast<int>(off + p->base));
        } else {
            ret = false;
        }
    }
    return ret;
}


bool ControlBoardWrapperMotorEncoders::setMotorEncoder(int m, const double val)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iMotEnc) {
        return p->iMotEnc->setMotorEncoder(static_cast<int>(off + p->base), val);
    }
    return false;
}


bool ControlBoardWrapperMotorEncoders::setMotorEncoders(const double* vals)
{
    bool ret = true;

    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->iMotEnc) {
            ret = ret && p->iMotEnc->setMotorEncoder(static_cast<int>(off + p->base), vals[l]);
        } else {
            ret = false;
        }
    }
    return ret;
}


bool ControlBoardWrapperMotorEncoders::setMotorEncoderCountsPerRevolution(int m, double cpr)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iMotEnc) {
        return p->iMotEnc->setMotorEncoderCountsPerRevolution(static_cast<int>(off + p->base), cpr);
    }
    return false;
}


bool ControlBoardWrapperMotorEncoders::getMotorEncoderCountsPerRevolution(int m, double* cpr)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iMotEnc) {
        return p->iMotEnc->getMotorEncoderCountsPerRevolution(static_cast<int>(off + p->base), cpr);
    }
    *cpr = 0.0;
    return false;
}


bool ControlBoardWrapperMotorEncoders::getMotorEncoder(int m, double* v)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iMotEnc) {
        return p->iMotEnc->getMotorEncoder(static_cast<int>(off + p->base), v);
    }
    *v = 0.0;
    return false;
}


bool ControlBoardWrapperMotorEncoders::getMotorEncoders(double* encs)
{

    auto* encValues = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iMotEnc) && (ret = p->iMotEnc->getMotorEncoders(encValues))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                encs[juser] = encValues[jdevice];
            }
        } else {
            printError("getMotorEncoders", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] encValues;
    return ret;
}


bool ControlBoardWrapperMotorEncoders::getMotorEncodersTimed(double* encs, double* t)
{
    auto* encValues = new double[device.maxNumOfJointsInDevices];
    auto* tValues = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iMotEnc) && (ret = p->iMotEnc->getMotorEncodersTimed(encValues, tValues))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                encs[juser] = encValues[jdevice];
                t[juser] = tValues[jdevice];
            }
        } else {
            printError("getMotorEncodersTimed", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] encValues;
    delete[] tValues;
    return ret;
}


bool ControlBoardWrapperMotorEncoders::getMotorEncoderTimed(int m, double* v, double* t)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iMotEnc) {
        return p->iMotEnc->getMotorEncoderTimed(static_cast<int>(off + p->base), v, t);
    }
    *v = 0.0;
    return false;
}


bool ControlBoardWrapperMotorEncoders::getMotorEncoderSpeed(int m, double* sp)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iMotEnc) {
        return p->iMotEnc->getMotorEncoderSpeed(static_cast<int>(off + p->base), sp);
    }
    *sp = 0.0;
    return false;
}


bool ControlBoardWrapperMotorEncoders::getMotorEncoderSpeeds(double* spds)
{
    auto* sValues = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iMotEnc) && (ret = p->iMotEnc->getMotorEncoderSpeeds(sValues))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                spds[juser] = sValues[jdevice];
            }
        } else {
            printError("getMotorEncoderSpeeds", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] sValues;
    return ret;
}


bool ControlBoardWrapperMotorEncoders::getMotorEncoderAcceleration(int m, double* acc)
{
    int off = device.lut[m].offset;
    size_t subIndex = device.lut[m].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iMotEnc) {
        return p->iMotEnc->getMotorEncoderAcceleration(static_cast<int>(off + p->base), acc);
    }
    *acc = 0.0;
    return false;
}


bool ControlBoardWrapperMotorEncoders::getMotorEncoderAccelerations(double* accs)
{
    auto* aValues = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iMotEnc) && (ret = p->iMotEnc->getMotorEncoderAccelerations(aValues))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                accs[juser] = aValues[jdevice];
            }
        } else {
            printError("getMotorEncoderAccelerations", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] aValues;
    return ret;
}


bool ControlBoardWrapperMotorEncoders::getNumberOfMotorEncoders(int* num)
{
    *num = controlledJoints;
    return true;
}
