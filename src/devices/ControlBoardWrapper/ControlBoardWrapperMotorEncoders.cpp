/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperMotorEncoders.h"

#include "ControlBoardWrapperLogComponent.h"

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
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->iMotEnc || !p->iMotEnc->resetMotorEncoder(static_cast<int>(off + p->base))) {
            return false;
        }
    }

    return true;
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
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->iMotEnc || !p->iMotEnc->setMotorEncoder(static_cast<int>(off + p->base), vals[l])) {
            return false;
        }
    }

    return true;
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
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->iMotEnc || !p->iMotEnc->getMotorEncoder(static_cast<int>(off + p->base), &encs[l])) {
            return false;
        }
    }

    return true;
}


bool ControlBoardWrapperMotorEncoders::getMotorEncodersTimed(double* encs, double* t)
{
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->iMotEnc || !p->iMotEnc->getMotorEncoderTimed(static_cast<int>(off + p->base), &encs[l], &t[l])) {
            return false;
        }
    }

    return true;
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
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->iMotEnc || !p->iMotEnc->getMotorEncoderSpeed(static_cast<int>(off + p->base), &spds[l])) {
            return false;
        }
    }

    return true;
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
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->iMotEnc || !p->iMotEnc->getMotorEncoderAcceleration(static_cast<int>(off + p->base), &accs[l])) {
            return false;
        }
    }

    return true;
}


bool ControlBoardWrapperMotorEncoders::getNumberOfMotorEncoders(int* num)
{
    *num = controlledJoints;
    return true;
}
