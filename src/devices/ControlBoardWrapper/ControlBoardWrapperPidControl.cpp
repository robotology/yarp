/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperPidControl.h"

#include "ControlBoardWrapperLogComponent.h"

using yarp::dev::Pid;
using yarp::dev::PidControlTypeEnum;


bool ControlBoardWrapperPidControl::setPid(const PidControlTypeEnum& pidtype, int j, const Pid& p)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER,
                "Joint number %d out of bound [0-%zu] for part %s",
                j,
                controlledJoints,
                partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* s = device.getSubdevice(subIndex);
    if (!s) {
        return false;
    }

    if (s->pid) {
        return s->pid->setPid(pidtype, static_cast<int>(off + s->base), p);
    }
    return false;
}


bool ControlBoardWrapperPidControl::setPids(const PidControlTypeEnum& pidtype, const Pid* ps)
{
    bool ret = true;

    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->pid) {
            ret = ret && p->pid->setPid(pidtype, static_cast<int>(off + p->base), ps[l]);
        } else {
            ret = false;
        }
    }
    return ret;
}


bool ControlBoardWrapperPidControl::setPidReference(const PidControlTypeEnum& pidtype, int j, double ref)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER,
                "Joint number %d out of bound [0-%zu] for part %s",
                j,
                controlledJoints,
                partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->pid) {
        return p->pid->setPidReference(pidtype, static_cast<int>(off + p->base), ref);
    }
    return false;
}


bool ControlBoardWrapperPidControl::setPidReferences(const PidControlTypeEnum& pidtype, const double* refs)
{
    bool ret = true;

    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->pid) {
            ret = ret && p->pid->setPidReference(pidtype, static_cast<int>(off + p->base), refs[l]);
        } else {
            ret = false;
        }
    }
    return ret;
}


bool ControlBoardWrapperPidControl::setPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double limit)
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

    if (p->pid) {
        return p->pid->setPidErrorLimit(pidtype, static_cast<int>(off + p->base), limit);
    }
    return false;
}


bool ControlBoardWrapperPidControl::setPidErrorLimits(const PidControlTypeEnum& pidtype, const double* limits)
{
    bool ret = true;

    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->pid) {
            ret = ret && p->pid->setPidErrorLimit(pidtype, static_cast<int>(off + p->base), limits[l]);
        } else {
            ret = false;
        }
    }
    return ret;
}


bool ControlBoardWrapperPidControl::getPidError(const PidControlTypeEnum& pidtype, int j, double* err)
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

    if (p->pid) {
        return p->pid->getPidError(pidtype, static_cast<int>(off + p->base), err);
    }
    *err = 0.0;
    return false;
}


bool ControlBoardWrapperPidControl::getPidErrors(const PidControlTypeEnum& pidtype, double* errs)
{
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->pid || !p->pid->getPidError(pidtype, static_cast<int>(off + p->base), &errs[l])) {
            return false;
        }
    }

    return true;
}


bool ControlBoardWrapperPidControl::getPidOutput(const PidControlTypeEnum& pidtype, int j, double* out)
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

    if (p->pid) {
        return p->pid->getPidOutput(pidtype, static_cast<int>(off + p->base), out);
    }
    *out = 0.0;
    return false;
}


bool ControlBoardWrapperPidControl::getPidOutputs(const PidControlTypeEnum& pidtype, double* outs)
{
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->pid || !p->pid->getPidOutput(pidtype, static_cast<int>(off + p->base), &outs[l])) {
            return false;
        }
    }

    return true;
}


bool ControlBoardWrapperPidControl::setPidOffset(const PidControlTypeEnum& pidtype, int j, double v)
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

    if (p->pid) {
        return p->pid->setPidOffset(pidtype, static_cast<int>(off + p->base), v);
    }
    return false;
}


bool ControlBoardWrapperPidControl::getPid(const PidControlTypeEnum& pidtype, int j, Pid* p)
{
    //#warning "check for max number of joints!?!?!"
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* s = device.getSubdevice(subIndex);
    if (!s) {
        return false;
    }

    if (s->pid) {
        return s->pid->getPid(pidtype, static_cast<int>(off + s->base), p);
    }
    return false;
}


bool ControlBoardWrapperPidControl::getPids(const PidControlTypeEnum& pidtype, Pid* pids)
{
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->pid || !p->pid->getPid(pidtype, static_cast<int>(off + p->base), &pids[l])) {
            return false;
        }
    }

    return true;
}


bool ControlBoardWrapperPidControl::getPidReference(const PidControlTypeEnum& pidtype, int j, double* ref)
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
    if (p->pid) {
        return p->pid->getPidReference(pidtype, static_cast<int>(off + p->base), ref);
    }
    return false;
}


bool ControlBoardWrapperPidControl::getPidReferences(const PidControlTypeEnum& pidtype, double* refs)
{
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->pid || !p->pid->getPidReference(pidtype, static_cast<int>(off + p->base), &refs[l])) {
            return false;
        }
    }

    return true;
}


bool ControlBoardWrapperPidControl::getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double* limit)
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

    if (p->pid) {
        return p->pid->getPidErrorLimit(pidtype, static_cast<int>(off + p->base), limit);
    }
    return false;
}


bool ControlBoardWrapperPidControl::getPidErrorLimits(const PidControlTypeEnum& pidtype, double* limits)
{
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->pid || !p->pid->getPidErrorLimit(pidtype, static_cast<int>(off + p->base), &limits[l])) {
            return false;
        }
    }

    return true;
}


bool ControlBoardWrapperPidControl::resetPid(const PidControlTypeEnum& pidtype, int j)
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

    if (p->pid) {
        return p->pid->resetPid(pidtype, static_cast<int>(off + p->base));
    }
    return false;
}


bool ControlBoardWrapperPidControl::disablePid(const PidControlTypeEnum& pidtype, int j)
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

    if (p->pid) {
        return p->pid->disablePid(pidtype, static_cast<int>(off + p->base));
    }
    return false;
}


bool ControlBoardWrapperPidControl::enablePid(const PidControlTypeEnum& pidtype, int j)
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

    if (p->pid) {
        return p->pid->enablePid(pidtype, static_cast<int>(off + p->base));
    }
    return false;
}


bool ControlBoardWrapperPidControl::isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool* enabled)
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

    if (p->pid) {
        return p->pid->isPidEnabled(pidtype, static_cast<int>(off + p->base), enabled);
    }

    return false;
}
