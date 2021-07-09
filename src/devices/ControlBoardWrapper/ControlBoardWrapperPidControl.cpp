/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapperPidControl.h"

#include "ControlBoardLogComponent.h"

using yarp::dev::Pid;
using yarp::dev::PidControlTypeEnum;


bool ControlBoardWrapperPidControl::setPid(const PidControlTypeEnum& pidtype, int j, const Pid& p)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARD,
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
        yCError(CONTROLBOARD,
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
        yCError(CONTROLBOARD, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
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
        yCError(CONTROLBOARD, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
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
    auto* errors = new double[device.maxNumOfJointsInDevices];

    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }
        if ((p->pid) && (ret = p->pid->getPidErrors(pidtype, errors))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                errs[juser] = errors[jdevice];
            }
        } else {
            printError("getPidErrors", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] errors;
    return ret;
}


bool ControlBoardWrapperPidControl::getPidOutput(const PidControlTypeEnum& pidtype, int j, double* out)
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

    if (p->pid) {
        return p->pid->getPidOutput(pidtype, static_cast<int>(off + p->base), out);
    }
    *out = 0.0;
    return false;
}


bool ControlBoardWrapperPidControl::getPidOutputs(const PidControlTypeEnum& pidtype, double* outs)
{
    auto* outputs = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->pid) && (ret = p->pid->getPidOutputs(pidtype, outputs))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                outs[juser] = outputs[jdevice];
            }
        } else {
            printError("getPidOutouts", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] outputs;
    return ret;
}


bool ControlBoardWrapperPidControl::setPidOffset(const PidControlTypeEnum& pidtype, int j, double v)
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
        yCError(CONTROLBOARD, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
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
    Pid* pids_device = new Pid[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->pid) && (ret = p->pid->getPids(pidtype, pids_device))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                pids[juser] = pids_device[jdevice];
            }
        } else {
            printError("getPids", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] pids_device;
    return ret;
}


bool ControlBoardWrapperPidControl::getPidReference(const PidControlTypeEnum& pidtype, int j, double* ref)
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
    if (p->pid) {
        return p->pid->getPidReference(pidtype, static_cast<int>(off + p->base), ref);
    }
    return false;
}


bool ControlBoardWrapperPidControl::getPidReferences(const PidControlTypeEnum& pidtype, double* refs)
{
    auto* references = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->pid) && (ret = p->pid->getPidReferences(pidtype, references))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                refs[juser] = references[jdevice];
            }
        } else {
            printError("getPidReferences", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] references;
    return ret;
}


bool ControlBoardWrapperPidControl::getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double* limit)
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

    if (p->pid) {
        return p->pid->getPidErrorLimit(pidtype, static_cast<int>(off + p->base), limit);
    }
    return false;
}


bool ControlBoardWrapperPidControl::getPidErrorLimits(const PidControlTypeEnum& pidtype, double* limits)
{
    auto* lims = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->pid) && (ret = p->pid->getPidErrorLimits(pidtype, lims))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                limits[juser] = lims[jdevice];
            }
        } else {
            printError("getPidErrorLimits", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] lims;
    return ret;
}


bool ControlBoardWrapperPidControl::resetPid(const PidControlTypeEnum& pidtype, int j)
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
        yCError(CONTROLBOARD, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
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
        yCError(CONTROLBOARD, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
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
        yCError(CONTROLBOARD, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
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
