/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperEncodersTimed.h"

#include "ControlBoardWrapperLogComponent.h"

bool ControlBoardWrapperEncodersTimed::resetEncoder(int j)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iJntEnc) {
        return p->iJntEnc->resetEncoder(static_cast<int>(off + p->base));
    }
    return false;
}


bool ControlBoardWrapperEncodersTimed::resetEncoders()
{
    bool ret = true;

    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->iJntEnc) {
            ret = ret && p->iJntEnc->resetEncoder(static_cast<int>(off + p->base));
        } else {
            ret = false;
        }
    }
    return ret;
}


bool ControlBoardWrapperEncodersTimed::setEncoder(int j, double val)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iJntEnc) {
        return p->iJntEnc->setEncoder(static_cast<int>(off + p->base), val);
    }
    return false;
}


bool ControlBoardWrapperEncodersTimed::setEncoders(const double* vals)
{
    bool ret = true;

    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->iJntEnc) {
            ret = ret && p->iJntEnc->setEncoder(static_cast<int>(off + p->base), vals[l]);
        } else {
            ret = false;
        }
    }
    return ret;
}


bool ControlBoardWrapperEncodersTimed::getEncoder(int j, double* v)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iJntEnc) {
        return p->iJntEnc->getEncoder(static_cast<int>(off + p->base), v);
    }
    *v = 0.0;
    return false;
}


bool ControlBoardWrapperEncodersTimed::getEncoders(double* encs)
{
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->iJntEnc || !p->iJntEnc->getEncoder(static_cast<int>(off + p->base), &encs[l])) {
            return false;
        }
    }

    return true;
}


bool ControlBoardWrapperEncodersTimed::getEncodersTimed(double* encs, double* t)
{
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->iJntEnc || !p->iJntEnc->getEncoderTimed(static_cast<int>(off + p->base), &encs[l], &t[l])) {
            return false;
        }
    }

    return true;
}


bool ControlBoardWrapperEncodersTimed::getEncoderTimed(int j, double* v, double* t)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iJntEnc) {
        return p->iJntEnc->getEncoderTimed(static_cast<int>(off + p->base), v, t);
    }
    *v = 0.0;
    return false;
}


bool ControlBoardWrapperEncodersTimed::getEncoderSpeed(int j, double* sp)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iJntEnc) {
        return p->iJntEnc->getEncoderSpeed(static_cast<int>(off + p->base), sp);
    }
    *sp = 0.0;
    return false;
}


bool ControlBoardWrapperEncodersTimed::getEncoderSpeeds(double* spds)
{
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->iJntEnc || !p->iJntEnc->getEncoderSpeed(static_cast<int>(off + p->base), &spds[l])) {
            return false;
        }
    }

    return true;
}


bool ControlBoardWrapperEncodersTimed::getEncoderAcceleration(int j, double* acc)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iJntEnc) {
        return p->iJntEnc->getEncoderAcceleration(static_cast<int>(off + p->base), acc);
    }
    *acc = 0.0;
    return false;
}


bool ControlBoardWrapperEncodersTimed::getEncoderAccelerations(double* accs)
{
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->iJntEnc || !p->iJntEnc->getEncoderAcceleration(static_cast<int>(off + p->base), &accs[l])) {
            return false;
        }
    }

    return true;
}
