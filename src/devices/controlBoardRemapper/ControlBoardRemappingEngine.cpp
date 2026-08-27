/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardRemapper.h"
#include "ControlBoardRemapperHelpers.h"
#include "ControlBoardRemapperLogComponent.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <mutex>
#include <cassert>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

void ControlBoardRemappingEngine::setAxesNames(const std::vector<std::string>& axesNames)
{
    this->axesNames = axesNames;
    setNrOfControlledAxes(axesNames.size());
}

void ControlBoardRemappingEngine::setNrOfControlledAxes(const size_t nrOfControlledAxes)
{
    controlledJoints = nrOfControlledAxes;
    buffers.controlBoardModes.resize(nrOfControlledAxes,0);
    buffers.dummyBuffer.resize(nrOfControlledAxes,0.0);
}

bool ControlBoardRemappingEngine::attachAllDevices(const PolyDriverList &polylist)
{
    bool ok = attachAllUsingAxesNames(polylist);

    //check if all devices are attached to the driver
    bool ready=true;
    for(unsigned int k=0; k<remappedControlBoards.getNrOfSubControlBoards(); k++)
    {
        if (!remappedControlBoards.subdevices[k].isAttached())
        {
            ready=false;
        }
    }

    if (!ready)
    {
        yCError(CONTROLBOARDREMAPPER, "AttachAll failed, some subdevice was not found or its attach failed");
        return false;
    }

    if( ok )
    {
        // configure the buffers, now that we have all the information about the attached devices
        allJointsBuffers.configure(remappedControlBoards);
        selectedJointsBuffers.configure(remappedControlBoards);
    }

    return ok;
}

// First we store a map between each axes name
// in the passed PolyDriverList and the device in which they belong and their index
struct axisLocation
{
    std::string subDeviceKey;
    size_t indexOfSubDeviceInPolyDriverList;
    int indexInSubDevice;
};


bool ControlBoardRemappingEngine::attachAllUsingAxesNames(const PolyDriverList& polylist)
{
    std::map<std::string, axisLocation> axesLocationMap;

    for(int p=0;p<polylist.size();p++)
    {
        // If there is a device with a specific device key, use it
        // as a calibrator, otherwise rely on the subcontrolboards
        // as usual
        std::string deviceKey=polylist[p]->key;
        if(deviceKey == "Calibrator" || deviceKey == "calibrator")
        {
            // Set the IRemoteCalibrator interface, the wrapper must point to the calibrator rdevice
            yarp::dev::IRemoteCalibrator *calibrator;
            polylist[p]->poly->view(calibrator);

            IRemoteCalibrator::setCalibratorDevice(calibrator);
            continue;
        }

        // find if one of the desired axis is in this device
        yarp::dev::IAxisInfo *iaxinfos = nullptr;
        yarp::dev::IEncoders *iencs = nullptr;
        polylist[p]->poly->view(iaxinfos);
        polylist[p]->poly->view(iencs);

        if( !iencs ||
            !iaxinfos )
        {
            yCError(CONTROLBOARDREMAPPER) << "sub-device" << deviceKey << "does not implemented the required IAxisInfo or IEncoders interfaces";
            return false;
        }

        size_t nrOfSubdeviceAxes;
        bool ok = iencs->getAxes(nrOfSubdeviceAxes);

        if( !ok )
        {
            yCError(CONTROLBOARDREMAPPER) << "sub-device" << deviceKey << "does not implemented the required getAxes method";
            return false;
        }

        for(size_t axInSubDevice =0; axInSubDevice < nrOfSubdeviceAxes; axInSubDevice++)
        {
            std::string axNameYARP;
            ok = iaxinfos->getAxisName(axInSubDevice,axNameYARP);

            std::string axName = axNameYARP;

            if( !ok )
            {
                yCError(CONTROLBOARDREMAPPER) << "sub-device" << deviceKey << "does not implemented the required getAxisName method";
                return false;
            }

            auto it = axesLocationMap.find(axName);
            if( it != axesLocationMap.end() )
            {
                yCError(CONTROLBOARDREMAPPER)
                    << "multiple axes have the same name" << axName
                    << "on on device " << polylist[p]->key << "with index" << axInSubDevice
                    <<"and another on device" << it->second.subDeviceKey << "with index" << it->second.indexInSubDevice;
                return false;
            }

            axisLocation newLocation;
            newLocation.subDeviceKey = polylist[p]->key;
            newLocation.indexInSubDevice = axInSubDevice;
            newLocation.indexOfSubDeviceInPolyDriverList = p;
            axesLocationMap[axName] = newLocation;
        }
    }

    // We store the key of all the devices that we actually use in the remapped control device
    std::vector<std::string> subControlBoardsKeys;
    std::map<std::string, size_t> subControlBoardKey2IndexInPolyDriverList;
    std::map<std::string, size_t> subControlBoardKey2IndexInRemappedControlBoards;


    // Once we build the axis map, we build the mapping between the remapped axes and
    // the couple subControlBoard, axis in subControlBoard
    for(const auto& axesName : axesNames)
    {
        auto it = axesLocationMap.find(axesName);
        if( it == axesLocationMap.end() )
        {
            yCError(CONTROLBOARDREMAPPER)
                << "axis " << axesName
                << "specified in axesNames was not found in the axes of the controlboards passed to attachAll, attachAll failed.";
            return false;
        }

        axisLocation loc = it->second;
        std::string key = loc.subDeviceKey;

        if(std::find(subControlBoardsKeys.begin(), subControlBoardsKeys.end(), key) == subControlBoardsKeys.end())
        {
            /* subControlBoardsKeys does not contain key */
            subControlBoardKey2IndexInRemappedControlBoards[key] = subControlBoardsKeys.size();
            subControlBoardsKeys.push_back(key);
            subControlBoardKey2IndexInPolyDriverList[key] = loc.indexOfSubDeviceInPolyDriverList;
        }
    }

    assert(controlledJoints == (int) axesNames.size());

    // We have now the number of controlboards to attach to
    size_t nrOfSubControlBoards = subControlBoardsKeys.size();
    remappedControlBoards.lut.resize(controlledJoints);
    remappedControlBoards.subdevices.resize(nrOfSubControlBoards);

    // Open the controlboards
    for(size_t ctrlBrd=0; ctrlBrd < nrOfSubControlBoards; ctrlBrd++)
    {
        size_t p = subControlBoardKey2IndexInPolyDriverList[subControlBoardsKeys[ctrlBrd]];
        RemappedSubControlBoard *tmpDevice = remappedControlBoards.getSubControlBoard(ctrlBrd);
        tmpDevice->setVerbose(_verb);
        tmpDevice->id = subControlBoardsKeys[ctrlBrd];
        bool ok = tmpDevice->attach(polylist[p]->poly,subControlBoardsKeys[ctrlBrd]);

        if( !ok )
        {
            return false;
        }
    }


    for(size_t l=0; l < axesNames.size(); l++)
    {
        axisLocation loc = axesLocationMap[axesNames[l]];
        remappedControlBoards.lut[l].subControlBoardIndex = subControlBoardKey2IndexInRemappedControlBoards[loc.subDeviceKey];
        remappedControlBoards.lut[l].axisIndexInSubControlBoard = (size_t)loc.indexInSubDevice;
    }

    return true;
}


bool ControlBoardRemappingEngine::detachAllDevices()
{
    //check if we already instantiated a subdevice previously
    int devices=remappedControlBoards.getNrOfSubControlBoards();
    for (int k = 0; k < devices; k++) {
        remappedControlBoards.getSubControlBoard(k)->detach();
    }

    IRemoteCalibrator::releaseCalibratorDevice();
    return true;
}


//////////////////////////////////////////////////////////////////////////////
/// ControlBoard methods
//////////////////////////////////////////////////////////////////////////////

//
//  IPid Interface
//

ReturnValue ControlBoardRemappingEngine::getAvailablePids(int j, std::vector<yarp::dev::PidControlTypeEnum>& avail)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
    if (!p) {
        return ReturnValue::return_code::return_value_error_generic;
    }

    ReturnValue ok = ReturnValue_ok;

    if (p->pid) {
        ok = p->pid->getAvailablePids(off, avail);
    }
    else {
        ok = ReturnValue::return_code::return_value_error_generic;
    }

    return ok;
}

ReturnValue ControlBoardRemappingEngine::setPid(const PidControlTypeEnum& pidtype, int j, const Pid &p)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *s=remappedControlBoards.getSubControlBoard(subIndex);
    if (!s)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (s->pid)
    {
        return s->pid->setPid(pidtype, off, p);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setPids(const PidControlTypeEnum& pidtype, const Pid *ps)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;

        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->pid)
        {
            ReturnValue ok = p->pid->setPid(pidtype, off, ps[l]);
            ret=ret&&ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::setPidReference(const PidControlTypeEnum& pidtype, int j, double ref)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pid)
    {
        return p->pid->setPidReference(pidtype, off, ref);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setPidReferences(const PidControlTypeEnum& pidtype, const double *refs)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;

        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->pid)
        {
            ReturnValue ok = p->pid->setPidReference(pidtype, off, refs[l]);
            ret=ret&&ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::setPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double limit)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pid)
    {
        return p->pid->setPidErrorLimit(pidtype, off, limit);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setPidErrorLimits(const PidControlTypeEnum& pidtype, const double *limits)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;

        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->pid)
        {
            ReturnValue ok = p->pid->setPidErrorLimit(pidtype, off, limits[l]);
            ret=ret&&ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::getPidError(const PidControlTypeEnum& pidtype, int j, double *err)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pid)
    {
        return p->pid->getPidError(pidtype, off, err);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getPidErrors(const PidControlTypeEnum& pidtype, double *errs)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->pid)
        {
            ReturnValue ok = p->pid->getPidError(pidtype, off, errs+l);
            ret=ret&&ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::getPidOutput(const PidControlTypeEnum& pidtype, int j, double *out)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pid)
    {
        return p->pid->getPidOutput(pidtype, off, out);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getPidOutputs(const PidControlTypeEnum& pidtype, double *outs)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;

        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->pid)
        {
            ret=ret&&p->pid->getPidOutput(pidtype, off, outs+l);
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::setPidOffset(const PidControlTypeEnum& pidtype, int j, double v)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pid)
    {
        return p->pid->setPidOffset(pidtype, off, v);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getPidOffset(const PidControlTypeEnum& pidtype, int j, double& v)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *s=remappedControlBoards.getSubControlBoard(subIndex);
    if (!s)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (s->pid)
    {
        return s->pid->getPidOffset(pidtype, off, v);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setPidFeedforward(const PidControlTypeEnum& pidtype, int j, double v)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pid)
    {
        return p->pid->setPidFeedforward(pidtype, off, v);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getPidFeedforward(const PidControlTypeEnum& pidtype, int j, double& v)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *s=remappedControlBoards.getSubControlBoard(subIndex);
    if (!s)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (s->pid)
    {
        return s->pid->getPidFeedforward(pidtype, off, v);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getPid(const PidControlTypeEnum& pidtype, int j, Pid *p)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *s=remappedControlBoards.getSubControlBoard(subIndex);
    if (!s)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (s->pid)
    {
        return s->pid->getPid(pidtype, off, p);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getPids(const PidControlTypeEnum& pidtype, Pid *pids)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->pid)
        {
            ReturnValue ok = p->pid->getPid(pidtype, off, pids+l);
            ret = ok && ret;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getPidReference(const PidControlTypeEnum& pidtype, int j, double *ref)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pid)
    {
        return p->pid->getPidReference(pidtype, off, ref);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getPidReferences(const PidControlTypeEnum& pidtype, double *refs)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;

        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->pid)
        {
            ReturnValue ok = p->pid->getPidReference(pidtype, off, refs+l);
            ret=ret && ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double *limit)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pid)
    {
        return p->pid->getPidErrorLimit(pidtype, off, limit);
    }
    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getPidErrorLimits(const PidControlTypeEnum& pidtype, double *limits)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;

        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->pid)
        {
            ReturnValue ok = p->pid->getPidErrorLimit(pidtype, off, limits+l);
            ret=ret&&ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::resetPid(const PidControlTypeEnum& pidtype, int j)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pid)
    {
        return p->pid->resetPid(pidtype, off);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::disablePid(const PidControlTypeEnum& pidtype, int j)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    return p->pid->disablePid(pidtype, off);
}

ReturnValue ControlBoardRemappingEngine::enablePid(const PidControlTypeEnum& pidtype, int j)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pid)
    {
        return p->pid->enablePid(pidtype, off);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool& enabled)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pid)
    {
        return p->pid->isPidEnabled(pidtype, off, enabled);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getPidExtraInfo(const PidControlTypeEnum& pidtype, int j, PidExtraInfo& units)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pid)
    {
        return p->pid->getPidExtraInfo(pidtype, off, units);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getPidExtraInfos(const PidControlTypeEnum& pidtype, std::vector<PidExtraInfo>& units)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->pid)
        {
            ReturnValue ok = p->pid->getPidExtraInfo(pidtype, off, units[l]);
            ret = ok && ret;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }

    return ret;
}

/* IPositionControl */
ReturnValue ControlBoardRemappingEngine::getAxes(size_t& ax)
{
    ax=controlledJoints;
    return ReturnValue_ok;
}

ReturnValue ControlBoardRemappingEngine::positionMove(int j, double ref)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pos)
    {
        return p->pos->positionMove(off, ref);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::positionMove(const double *refs)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(refs,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;
        if (p->pos) {
            ok = p->pos->positionMove(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                        allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                        allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else {
            ok = ReturnValue::return_code::return_value_error_generic;
        }
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::positionMove(const int n_joints, const int *joints, const double *refs)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(refs,n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;
        if (p->pos) {
            ok = p->pos->positionMove(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                        selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                        selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else {
            ok = ReturnValue::return_code::return_value_error_generic;
        }
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getTargetPosition(const int j, double* ref)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pos)
    {
        ReturnValue ret = p->pos->getTargetPosition(off, ref);
        return ret;
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getTargetPositions(double *spds)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;

        if( p->pos )
        {
            ok = p->pos->getTargetPositions(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                             allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                             allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }

        ret = ret && ok;
    }

    allJointsBuffers.fillCompleteJointVectorFromSubControlBoardBuffers(spds,remappedControlBoards);

    return ret;
}


ReturnValue ControlBoardRemappingEngine::getTargetPositions(const int n_joints, const int *joints, double *targets)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    // Resize the input buffers
    selectedJointsBuffers.resizeSubControlBoardBuffers(n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;

        if( p->pos )
        {
            ok = p->pos->getTargetPositions(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                             selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                             selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }

        ret = ret && ok;
    }

    selectedJointsBuffers.fillArbitraryJointVectorFromSubControlBoardBuffers(targets,n_joints,joints,remappedControlBoards);

    return ret;
}

ReturnValue ControlBoardRemappingEngine::relativeMove(int j, double delta)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pos)
    {
        return p->pos->relativeMove(off, delta);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::relativeMove(const double *deltas)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(deltas,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);
        ReturnValue ok = ReturnValue_ok;
        if (p->pos) {
            ok = p->pos->relativeMove(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                        allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                        allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else {
            ok = ReturnValue::return_code::return_value_error_generic;
        }
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::relativeMove(const int n_joints, const int *joints, const double *deltas)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(deltas,n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);
        ReturnValue ok = ReturnValue_ok;
        if (p->pos) {
            ok = p->pos->relativeMove(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                        selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                        selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else {
            ok = ReturnValue::return_code::return_value_error_generic;
        }
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::checkMotionDone(int j, bool& flag)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pos)
    {
        return p->pos->checkMotionDone(off, flag);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::checkMotionDone(bool& flag)
{
    ReturnValue ret=ReturnValue_ok;
    flag=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->pos)
        {
            bool singleJointMotionDone =false;
            ReturnValue ok = p->pos->checkMotionDone(off, singleJointMotionDone);
            ret = ret && ok;
            flag = flag  && singleJointMotionDone;
        }
        else
        {
            ret = ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::checkMotionDone(const std::vector<int>& joints, bool& flag)
{
    ReturnValue ret=ReturnValue_ok;
    flag=true;

    for(int j=0;j<joints.size();j++)
    {
        int l = joints[j];

        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->pos)
        {
            bool singleJointMotionDone =false;
            ReturnValue ok = p->pos->checkMotionDone(off, singleJointMotionDone);
            ret = ret && ok;
            flag = flag  && singleJointMotionDone;
        }
        else
        {
            ret = ReturnValue::return_code::return_value_error_generic;
        }
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setTrajSpeed(int j, double sp)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pos)
    {
        return p->pos->setTrajSpeed(off, sp);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setTrajSpeeds(const double *spds)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(spds,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;
        if (p->pos) {
            ok = p->pos->setTrajSpeeds(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                        allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                        allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else {
            ok = ReturnValue::return_code::return_value_error_generic;
        }
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setTrajSpeeds(const int n_joints, const int *joints, const double *spds)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(spds,n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;
        if (p->pos) {
            ok = p->pos->setTrajSpeeds(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                        selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                        selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else {
            ok = ReturnValue::return_code::return_value_error_generic;
        }
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setTrajAcceleration(int j, double acc)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pos)
    {
        return p->pos->setTrajAcceleration(off, acc);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setTrajAccelerations(const double *accs)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(accs,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;
        if (p->pos) {
            ok = p->pos->setTrajAccelerations(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                                allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                                allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else {
            ok = ReturnValue::return_code::return_value_error_generic;
        }
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setTrajAccelerations(const int n_joints, const int *joints, const double *accs)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(accs,n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;
        if (p->pos) {
            ok = p->pos->setTrajAccelerations(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                               selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                               selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else {
            ok = ReturnValue::return_code::return_value_error_generic;
        }
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getTrajSpeed(int j, double *ref)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pos)
    {
        return p->pos->getTrajSpeed(off, ref);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getTrajSpeeds(double *spds)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;
        if( p->pos )
        {
            ok = p->pos->getTrajSpeeds(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                       allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                       allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }

        ret = ret && ok;
    }

    allJointsBuffers.fillCompleteJointVectorFromSubControlBoardBuffers(spds,remappedControlBoards);

    return ret;
}


ReturnValue ControlBoardRemappingEngine::getTrajSpeeds(const int n_joints, const int *joints, double *spds)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    // Resize the input buffers
    selectedJointsBuffers.resizeSubControlBoardBuffers(n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;

        if( p->pos )
        {
            ok = p->pos->getTrajSpeeds(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                            selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                            selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }

        ret = ret && ok;
    }

    selectedJointsBuffers.fillArbitraryJointVectorFromSubControlBoardBuffers(spds,n_joints,joints,remappedControlBoards);

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getTrajAcceleration(int j, double *acc)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pos)
    {
        return p->pos->getTrajAcceleration(off, acc);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getTrajAccelerations(double *accs)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;

        if( p->pos )
        {
            ok = p->pos->getTrajAccelerations(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                              allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                              allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }

        ret = ret && ok;
    }

    allJointsBuffers.fillCompleteJointVectorFromSubControlBoardBuffers(accs,remappedControlBoards);

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getTrajAccelerations(const int n_joints, const int *joints, double *accs)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    // Resize the input buffers
    selectedJointsBuffers.resizeSubControlBoardBuffers(n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;
        if( p->pos )
        {
            ok = p->pos->getTrajAccelerations(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                              selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                              selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }

        ret = ret && ok;
    }

    selectedJointsBuffers.fillArbitraryJointVectorFromSubControlBoardBuffers(accs,n_joints,joints,remappedControlBoards);

    return ret;
}

ReturnValue ControlBoardRemappingEngine::stop(int j)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->pos)
    {
        return p->pos->stop(off);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::stop()
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;
        if (p->pos) {
            ok = p->pos->stop(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                     allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::stop(const int n_joints, const int *joints)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);
    std::lock_guard<std::mutex> lock2(buffers.mutex);


    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(buffers.dummyBuffer.data(),n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);
        ReturnValue ok = ReturnValue_ok;
        if (p->pos) {
            ok = p->pos->stop(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }
        ret = ret && ok;
    }

    return ret;
}

/* IJointFaultControl */
ReturnValue ControlBoardRemappingEngine::getLastJointFault(int j, int& fault, std::string& message)
{
    int off = (int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard* p = remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iFault)
    {
        return p->iFault->getLastJointFault(off, fault, message);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

/* IVelocityControl */

ReturnValue ControlBoardRemappingEngine::velocityMove(int j, double v)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->vel)
    {
        return p->vel->velocityMove(off, v);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::velocityMove(const double *v)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(v,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;
        if (p->vel) {
            ok = p->vel->velocityMove(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                            allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                            allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else {
            ok = ReturnValue::return_code::return_value_error_generic;
        }
        ret = ret && ok;
    }

    return ret;
}

/* IEncoders */
ReturnValue ControlBoardRemappingEngine::resetEncoder(int j)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iJntEnc)
    {
        return p->iJntEnc->resetEncoder(off);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::resetEncoders()
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iJntEnc)
        {
            ReturnValue ok = p->iJntEnc->resetEncoder(off);
            ret = ret && ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::setEncoder(int j, double val)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iJntEnc)
    {
        return p->iJntEnc->setEncoder(off,val);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setEncoders(const double *vals)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off = (int) remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex = remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iJntEnc)
        {
            ReturnValue ok = p->iJntEnc->setEncoder(off, vals[l]);
            ret = ret && ok;
        }
        else
        {
            ret = ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::getEncoder(int j, double *v)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iJntEnc)
    {
        return p->iJntEnc->getEncoder(off, v);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getEncoders(double *encs)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iJntEnc)
        {
            ReturnValue ok = p->iJntEnc->getEncoder(off, encs+l);
            ret = ret && ok;
        }
        else
        {
            ret = ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::getEncodersTimed(double *encs, double *t)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iJntEnc)
        {
            ReturnValue ok = p->iJntEnc->getEncoderTimed(off, encs+l, t+l);
            ret = ret && ok;
        }
        else
        {
            ret = ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::getEncoderTimed(int j, double *v, double *t)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iJntEnc)
    {
        return p->iJntEnc->getEncoderTimed(off, v, t);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getEncoderSpeed(int j, double *sp)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iJntEnc)
    {
        return p->iJntEnc->getEncoderSpeed(off, sp);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getEncoderSpeeds(double *spds)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iJntEnc)
        {
            ReturnValue ok = p->iJntEnc->getEncoderSpeed(off, spds+l);
            ret = ret && ok;
        }
        else
        {
            ret = ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::getEncoderAcceleration(int j, double *acc)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iJntEnc)
    {
        return p->iJntEnc->getEncoderAcceleration(off,acc);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getEncoderAccelerations(double *accs)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iJntEnc)
        {
            ReturnValue ok = p->iJntEnc->getEncoderAcceleration(off, accs+l);
            ret = ret && ok;
        }
        else
        {
            ret = ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

/* IMotor */
ReturnValue ControlBoardRemappingEngine::getNumberOfMotors(int *num)
{
    *num=controlledJoints;
    return ReturnValue_ok;
}

ReturnValue ControlBoardRemappingEngine::getTemperature(int m, double* val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->imotor)
    {
        return p->imotor->getTemperature(off, val);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getTemperatures(double *vals)
{
    ReturnValue ret=ReturnValue_ok;
    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->imotor)
        {
            ret=ret&&p->imotor->getTemperature(off, vals+l);
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::getTemperatureLimit(int m, double* val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->imotor)
    {
        return p->imotor->getTemperatureLimit(off, val);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setTemperatureLimit (int m, const double val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->imotor)
    {
        return p->imotor->setTemperatureLimit(off,val);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getGearboxRatio(int m, double* val)
{
    int off = (int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex = remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->imotor)
    {
        return p->imotor->getGearboxRatio(off, val);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setGearboxRatio(int m, const double val)
{
    int off = (int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex = remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->imotor)
    {
        return p->imotor->setGearboxRatio(off, val);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

/* IRemoteVariables */
ReturnValue ControlBoardRemappingEngine::getRemoteVariable(std::string key, yarp::os::Bottle& val)
{
    yCWarning(CONTROLBOARDREMAPPER, "getRemoteVariable is not properly implemented, use at your own risk.");

    ReturnValue b = ReturnValue_ok;

    for (unsigned int i = 0; i < remappedControlBoards.getNrOfSubControlBoards(); i++)
    {
        RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(i);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (!p->iVar)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        yarp::os::Bottle tmpval;
        ReturnValue ok = p->iVar->getRemoteVariable(key, tmpval);

        if (ok)
        {
            val.append(tmpval);
        }

        b = b && ok;
    }

    return b;
}

ReturnValue ControlBoardRemappingEngine::setRemoteVariable(std::string key, const yarp::os::Bottle& val)
{
    yCWarning(CONTROLBOARDREMAPPER, "setRemoteVariable is not properly implemented, use at your own risk.");

    size_t bottle_size = val.size();
    size_t device_size = remappedControlBoards.getNrOfSubControlBoards();
    if (bottle_size != device_size)
    {
        yCError(CONTROLBOARDREMAPPER, "ControlBoardRemappingEngine::setRemoteVariable bottle_size != device_size failure");
        return ReturnValue::return_code::return_value_error_generic;
    }

    ReturnValue b = ReturnValue_ok;
    for (unsigned int i = 0; i < device_size; i++)
    {
        RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(i);
        if (!p)
        {
            yCError(CONTROLBOARDREMAPPER, "ControlBoardRemappingEngine::setRemoteVariable !p failure");
            ReturnValue::return_code::return_value_error_generic;
        }
        if (!p->iVar)
        {
            yCError(CONTROLBOARDREMAPPER, "ControlBoardRemappingEngine::setRemoteVariable !p->iVar failure");
            ReturnValue::return_code::return_value_error_generic;
        }
        Bottle* partial_val = val.get(i).asList();
        if (partial_val)
        {
            b &= p->iVar->setRemoteVariable(key, *partial_val);
        }
        else
        {
            yCError(CONTROLBOARDREMAPPER, "ControlBoardRemappingEngine::setRemoteVariable general failure");
            return ReturnValue::return_code::return_value_error_generic;
        }
    }

    return b;
}

ReturnValue ControlBoardRemappingEngine::getRemoteVariablesList(yarp::os::Bottle* listOfKeys)
{
    yCWarning(CONTROLBOARDREMAPPER, "getRemoteVariablesList is not properly implemented, use at your own risk.");

    size_t subIndex = remappedControlBoards.lut[0].subControlBoardIndex;
    RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iVar)
    {
        return p->iVar->getRemoteVariablesList(listOfKeys);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

/* IMotorEncoders */

ReturnValue ControlBoardRemappingEngine::resetMotorEncoder(int m)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iMotEnc)
    {
        return p->iMotEnc->resetMotorEncoder(off);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::resetMotorEncoders()
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iMotEnc)
        {
            ReturnValue ok = p->iMotEnc->resetMotorEncoder(off);
            ret= ret && ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setMotorEncoder(int m, const double val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iMotEnc)
    {
        return p->iMotEnc->setMotorEncoder(off,val);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setMotorEncoders(const double *vals)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iMotEnc)
        {
            ReturnValue ok = p->iMotEnc->setMotorEncoder(off, vals[l]);
            ret = ret && ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setMotorEncoderCountsPerRevolution(int m, double cpr)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iMotEnc)
    {
        return p->iMotEnc->setMotorEncoderCountsPerRevolution(off,cpr);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getMotorEncoderCountsPerRevolution(int m, double *cpr)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoderCountsPerRevolution(off, cpr);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getMotorEncoder(int m, double *v)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoder(off, v);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getMotorEncoders(double *encs)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p) {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iMotEnc)
        {
            ret=ret&&p->iMotEnc->getMotorEncoder(off, encs+l);
        } else {
            ret = ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::getMotorEncodersTimed(double *encs, double *t)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iMotEnc)
        {
            ReturnValue ok = p->iMotEnc->getMotorEncoderTimed(off, encs, t);
            ret = ret && ok;
        }
        else
        {
            ret = ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::getMotorEncoderTimed(int m, double *v, double *t)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoderTimed(off, v, t);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getMotorEncoderSpeed(int m, double *sp)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoderSpeed(off, sp);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getMotorEncoderSpeeds(double *spds)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off = (int) remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex = remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iMotEnc)
        {
            ReturnValue ok = p->iMotEnc->getMotorEncoderSpeed(off, spds + l);
            ret = ret && ok;
        }
        else
        {
            ret = ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::getMotorEncoderAcceleration(int m, double *acc)
{
    int off = (int) remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex = remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoderAcceleration(off,acc);
    }
    *acc=0.0;
    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getMotorEncoderAccelerations(double *accs)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iMotEnc)
        {
            ReturnValue ok = p->iMotEnc->getMotorEncoderAcceleration(off, accs+l);
            ret=ret && ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }

    return ret;
}


ReturnValue ControlBoardRemappingEngine::getNumberOfMotorEncoders(int *num)
{
    *num=controlledJoints;
    return ReturnValue_ok;
}

/* IAmplifierControl */
ReturnValue ControlBoardRemappingEngine::enableAmp(int j)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->amp)
    {
        return p->amp->enableAmp(off);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::disableAmp(int j)
{
    return this->setControlMode(j, yarp::dev::SelectableControlModeEnum::VOCAB_CM_IDLE);
}

ReturnValue ControlBoardRemappingEngine::getAmpStatus(int *st)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->amp)
        {
            ReturnValue ok = p->amp->getAmpStatus(off, st+l);
            ret = ret && ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getAmpStatus(int j, int *v)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->amp)
    {
        return p->amp->getAmpStatus(off,v);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setMaxCurrent(int j, double v)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->amp)
    {
        return p->amp->setMaxCurrent(off,v);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getMaxCurrent(int j, double* v)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->amp)
    {
        return p->amp->getMaxCurrent(off,v);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getNominalCurrent(int m, double *val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if(!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if(!p->amp)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    return p->amp->getNominalCurrent(off, val);
}

ReturnValue ControlBoardRemappingEngine::getPeakCurrent(int m, double *val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if(!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if(!p->amp)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    return p->amp->getPeakCurrent(off, val);
}

ReturnValue ControlBoardRemappingEngine::setPeakCurrent(int m, const double val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (!p->amp)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    return p->amp->setPeakCurrent(off, val);
}

ReturnValue ControlBoardRemappingEngine::setNominalCurrent(int m, const double val)
{
    int off = (int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex = remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (!p->amp)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    return p->amp->setNominalCurrent(off, val);
}

ReturnValue ControlBoardRemappingEngine::getPWM(int m, double* val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if(!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if(!p->amp)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    return p->amp->getPWM(off, val);
}
ReturnValue ControlBoardRemappingEngine::getPWMLimit(int m, double* val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if(!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if(!p->amp)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    return p->amp->getPWMLimit(off, val);
}

ReturnValue ControlBoardRemappingEngine::setPWMLimit(int m, const double val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (!p->amp)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    return p->amp->setPWMLimit(off, val);
}

ReturnValue ControlBoardRemappingEngine::getPowerSupplyVoltage(int m, double* val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if(!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if(!p->amp)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    return p->amp->getPowerSupplyVoltage(off, val);
}


/* IControlLimits */

ReturnValue ControlBoardRemappingEngine::setPosLimits(int j, double min, double max)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->lim)
    {
        return p->lim->setPosLimits(off,min, max);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getPosLimits(int j, double *min, double *max)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->lim)
    {
        return p->lim->getPosLimits(off,min, max);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setVelLimits(int j, double min, double max)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (!p->lim)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    return p->lim->setVelLimits(off,min, max);
}

ReturnValue ControlBoardRemappingEngine::getVelLimits(int j, double *min, double *max)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if(!p->lim)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    return p->lim->getVelLimits(off,min, max);
}

/* IRemoteCalibrator */
IRemoteCalibrator *ControlBoardRemappingEngine::getCalibratorDevice()
{
    return yarp::dev::IRemoteCalibrator::getCalibratorDevice();
}

ReturnValue ControlBoardRemappingEngine::isCalibratorDevicePresent(bool *isCalib)
{
    return yarp::dev::IRemoteCalibrator::isCalibratorDevicePresent(isCalib);
}

ReturnValue ControlBoardRemappingEngine::calibrateSingleJoint(int j)
{
    if(!getCalibratorDevice())
    {
        int off = (int) remappedControlBoards.lut[j].axisIndexInSubControlBoard;
        size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;

        RemappedSubControlBoard *s = remappedControlBoards.getSubControlBoard(subIndex);
        if (!s)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (s->remcalib)
        {
            return s->remcalib->calibrateSingleJoint(off);
        }

        return ReturnValue::return_code::return_value_error_generic;
    }
    else
    {
        return IRemoteCalibrator::getCalibratorDevice()->calibrateSingleJoint(j);
    }
}

yarp::dev::ReturnValue ControlBoardRemappingEngine::calibrateWholePart()
{
    if(!getCalibratorDevice())
    {
        for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
        {
            RemappedSubControlBoard *s = remappedControlBoards.getSubControlBoard(ctrlBrd);
            if (!s)
            {
                return ReturnValue::return_code::return_value_error_generic;
            }

            if (s->remcalib)
            {
                return s->remcalib->calibrateWholePart();
            }
        }

        return ReturnValue::return_code::return_value_error_generic;
    }
    else
    {
        return IRemoteCalibrator::getCalibratorDevice()->calibrateWholePart();
    }
}

yarp::dev::ReturnValue ControlBoardRemappingEngine::homingSingleJoint(int j)
{
    if(!getCalibratorDevice())
    {
        int off = (int) remappedControlBoards.lut[j].axisIndexInSubControlBoard;
        size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;

        RemappedSubControlBoard *s = remappedControlBoards.getSubControlBoard(subIndex);
        if (!s)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (s->remcalib)
        {
            return s->remcalib->homingSingleJoint(off);
        }

        return ReturnValue::return_code::return_value_error_generic;
    }
    else
    {
        return IRemoteCalibrator::getCalibratorDevice()->homingSingleJoint(j);
    }
}

yarp::dev::ReturnValue ControlBoardRemappingEngine::homingWholePart()
{
    if(!getCalibratorDevice())
    {
        ReturnValue ret = ReturnValue_ok;
        for(int l=0;l<controlledJoints;l++)
        {
            ReturnValue ok = this->homingSingleJoint(l);
            ret = ret && ok;
        }

        return ret;
    }
    else
    {
        return IRemoteCalibrator::getCalibratorDevice()->homingWholePart();
    }
}

yarp::dev::ReturnValue ControlBoardRemappingEngine::parkSingleJoint(int j, bool _wait)
{
    if(!getCalibratorDevice())
    {
        int off = (int) remappedControlBoards.lut[j].axisIndexInSubControlBoard;
        size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;

        RemappedSubControlBoard *s = remappedControlBoards.getSubControlBoard(subIndex);
        if (!s)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (s->remcalib)
        {
            return s->remcalib->parkSingleJoint(off,_wait);
        }

        return ReturnValue::return_code::return_value_error_generic;
    }
    else
    {
        return getCalibratorDevice()->parkSingleJoint(j, _wait);
    }
}

ReturnValue ControlBoardRemappingEngine::parkWholePart()
{
    if(!getCalibratorDevice())
    {
        ReturnValue ret = ReturnValue_ok;

        for(int l=0; l<controlledJoints; l++)
        {
            bool _wait = false;
            ReturnValue ok = this->parkSingleJoint(l,_wait);
            ret = ret && ok;
        }

        return ret;
    }
    else
    {
        return getCalibratorDevice()->parkWholePart();
    }
}

yarp::dev::ReturnValue ControlBoardRemappingEngine::quitCalibrate()
{
    if(!getCalibratorDevice())
    {
        for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
        {
            RemappedSubControlBoard *s = remappedControlBoards.getSubControlBoard(ctrlBrd);
            if (!s)
            {
                return ReturnValue::return_code::return_value_error_generic;
            }

            if (s->remcalib)
            {
                return s->remcalib->quitCalibrate();
            }
        }

        return ReturnValue::return_code::return_value_error_generic;
    }
    else
    {
        return IRemoteCalibrator::getCalibratorDevice()->quitCalibrate();
    }
}

yarp::dev::ReturnValue ControlBoardRemappingEngine::quitPark()
{
    if(!getCalibratorDevice())
    {
        for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
        {
            RemappedSubControlBoard *s = remappedControlBoards.getSubControlBoard(ctrlBrd);
            if (!s)
            {
                return ReturnValue::return_code::return_value_error_generic;
            }

            if (s->remcalib)
            {
                return s->remcalib->quitPark();
            }
        }

        return ReturnValue::return_code::return_value_error_generic;
    }
    else
    {
        return IRemoteCalibrator::getCalibratorDevice()->quitPark();
    }
}


/* IControlCalibration */
ReturnValue ControlBoardRemappingEngine::calibrateAxisWithParams(int j, unsigned int ui, double v1, double v2, double v3)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->calib)
    {
        return p->calib->calibrateAxisWithParams(off, ui,v1,v2,v3);
    }
    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setCalibrationParameters(int j, const CalibrationParameters& params)
{
    int off = (int) remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->calib)
    {
        return p->calib->setCalibrationParameters(off, params);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::calibrationDone(int j)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->calib)
    {
        return p->calib->calibrationDone(off);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::abortPark()
{
    yCError(CONTROLBOARDREMAPPER, "Calling abortPark -- not implemented");
    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::abortCalibration()
{
    yCError(CONTROLBOARDREMAPPER, "Calling abortCalibration -- not implemented");
    return ReturnValue::return_code::return_value_error_generic;
}

/* IAxisInfo */

ReturnValue ControlBoardRemappingEngine::getAxisName(int j, std::string& name)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->info)
    {
        return p->info->getAxisName(off, name);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getJointType(int j, yarp::dev::JointTypeEnum& type)
{
    int off = (int) remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->info)
    {
        return p->info->getJointType(off, type);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getRefTorques(double *refs)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iTorque)
        {
            ReturnValue ok = p->iTorque->getRefTorque(off, refs+l);
            ret = ret && ok;
        }
        else
        {
            ret = ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
}

ReturnValue ControlBoardRemappingEngine::getRefTorque(int j, double *t)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iTorque)
    {
        return p->iTorque->getRefTorque(off, t);
    }
    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setRefTorques(const double *t)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(t,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok;

        if( p->iTorque )
        {
            ok = p->iTorque->setRefTorques(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                           allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                           allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }

        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setRefTorque(int j, double t)
{
    int off = (int) remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);
    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iTorque)
    {
        return p->iTorque->setRefTorque(off, t);
    }
    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setRefTorques(const int n_joints, const int *joints, const double *t)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(t,n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = p->iTorque->setRefTorques(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                            selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                            selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iTorque)
    {
        return p->iTorque->getMotorTorqueParams(off, params);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setMotorTorqueParams(int j,  const yarp::dev::MotorTorqueParameters params)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iTorque)
    {
        return p->iTorque->setMotorTorqueParams(off, params);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setImpedance(int j, double stiff, double damp)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iImpedance)
    {
        return p->iImpedance->setImpedance(off, stiff, damp);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setImpedanceOffset(int j, double offset)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iImpedance)
    {
        return p->iImpedance->setImpedanceOffset(off, offset);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getTorque(int j, double *t)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iTorque)
    {
        return p->iTorque->getTorque(off, t);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getTorques(double *t)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iTorque)
        {
            ReturnValue ok = p->iTorque->getTorque(off, t+l);
            ret = ret && ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }

    return ret;
 }

ReturnValue ControlBoardRemappingEngine::getTorqueRange(int j, double *min, double *max)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iTorque)
    {
        return p->iTorque->getTorqueRange(off, min, max);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getTorqueRanges(double *min, double *max)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iTorque)
        {
            ReturnValue ok = p->iTorque->getTorqueRange(off, min+l, max+l);
            ret = ret && ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }
    return ret;
 }

ReturnValue ControlBoardRemappingEngine::getImpedance(int j, double* stiff, double* damp)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iImpedance)
    {
        return p->iImpedance->getImpedance(off, stiff, damp);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getImpedanceOffset(int j, double* offset)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iImpedance)
    {
        return p->iImpedance->getImpedanceOffset(off, offset);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iImpedance)
    {
        return p->iImpedance->getCurrentImpedanceLimit(off, min_stiff, max_stiff, min_damp, max_damp);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getAvailableControlModes(int j, std::vector<yarp::dev::SelectableControlModeEnum>& avail)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
    if (!p) {
        return ReturnValue::return_code::return_value_error_generic;
    }

    ReturnValue ok = ReturnValue_ok;

    if (p->iMode) {
        ok = p->iMode->getAvailableControlModes(off, avail);
    }
    else {
        ok = ReturnValue::return_code::return_value_error_generic;
    }

    return ok;
}

ReturnValue ControlBoardRemappingEngine::getControlMode(int j, yarp::dev::ControlModeEnum& mode)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
    if (!p) {
        return ReturnValue::return_code::return_value_error_generic;
    }

    ReturnValue ok = ReturnValue_ok;

    if (p->iMode) {
        ok = p->iMode->getControlMode(off, mode);
    }
    else {
        ok = ReturnValue::return_code::return_value_error_generic;
    }

    return ok;

}

ReturnValue ControlBoardRemappingEngine::getControlModes(std::vector<yarp::dev::ControlModeEnum>& modes)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok;

        if( p->iMode )
        {
            ok = p->iMode->getControlModes( allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd],
                                            allJointsBuffers.m_bufferForSubControlBoardControlModesEnum[ctrlBrd]);
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }

        ret = ret && ok;
    }

    allJointsBuffers.fillCompleteJointVectorFromSubControlBoardBuffers(modes.data(),remappedControlBoards);

    return ret;
}

// IControlMode interface
ReturnValue ControlBoardRemappingEngine::getControlModes(const std::vector<int>& joints, std::vector<yarp::dev::ControlModeEnum>& modes)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    // Resize the input buffers
    selectedJointsBuffers.resizeSubControlBoardBuffers(joints.size(),joints.data(),remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok;

        if( p->iMode )
        {
            ok = p->iMode->getControlModes( selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd],
                                            selectedJointsBuffers.m_bufferForSubControlBoardControlModesEnum[ctrlBrd]);
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }

        ret = ret && ok;
    }

    selectedJointsBuffers.fillArbitraryJointVectorFromSubControlBoardBuffers(modes.data(),joints.size(),joints.data(),remappedControlBoards);

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setControlMode(int j, yarp::dev::SelectableControlModeEnum mode)
{
    ReturnValue ret = ReturnValue_ok;

    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->iMode)
    {
        ret = p->iMode->setControlMode(off, mode);
    }
    else
    {
        ret = ReturnValue::return_code::return_value_error_generic;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setControlModes(const std::vector<int>& joints, const std::vector<yarp::dev::SelectableControlModeEnum>& modes)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(modes.data(),joints.size(),joints.data(),remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;
        if (p->iMode) {
            ok = p->iMode->setControlModes(  selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd],
                                             selectedJointsBuffers.m_bufferForSubControlBoardSelectableControlModesEnum[ctrlBrd]);
        }
        else {
            ok = ReturnValue::return_code::return_value_error_generic;
        }
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setControlModes(const std::vector<yarp::dev::SelectableControlModeEnum>& modes)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(modes.data(),remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;
        if (p->iMode) {
            ok = p->iMode->setControlModes(allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd],
                                           allJointsBuffers.m_bufferForSubControlBoardSelectableControlModesEnum[ctrlBrd]);
        }
        else {
            ok = ReturnValue::return_code::return_value_error_generic;
        }
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setPosition(int j, double ref)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->posDir)
    {
        return p->posDir->setPosition(off, ref);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setPositions(const int n_joints, const int *joints, const double *dpos)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(dpos,n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;
        if (p->posDir) {
            ok = p->posDir->setPositions(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                          selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                          selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else {
            ok = ReturnValue::return_code::return_value_error_generic;
        }
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setPositions(const double *refs)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(refs,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;
        if (p->posDir) {
            ok = p->posDir->setPositions(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                          allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                          allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else {
            ok = ReturnValue::return_code::return_value_error_generic;
        }
        ret = ret && ok;
    }

    return ret;
}

yarp::os::Stamp ControlBoardRemappingEngine::getLastInputStamp()
{
    double averageTimestamp = 0.0;
    int collectedTimestamps = 0;

    for(int l=0;l<controlledJoints;l++)
    {
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return Stamp();
        }

        if(p->iTimed)
        {
            averageTimestamp = averageTimestamp + p->iTimed->getLastInputStamp().getTime();
            collectedTimestamps++;
        }
    }


    std::lock_guard<std::mutex> lock(buffers.mutex);

    if( collectedTimestamps > 0 )
    {
        buffers.stamp.update(averageTimestamp/collectedTimestamps);
    }
    else
    {
        buffers.stamp.update();
    }

    return buffers.stamp;
}

ReturnValue ControlBoardRemappingEngine::getRefPosition(const int j, double* ref)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->posDir)
    {
        ReturnValue ret = p->posDir->getRefPosition(off, ref);
        return ret;
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getRefPositions(double *spds)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;

        if( p->posDir )
        {
            ok = p->posDir->getRefPositions(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                            allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                            allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }

        ret = ret && ok;
    }

    allJointsBuffers.fillCompleteJointVectorFromSubControlBoardBuffers(spds,remappedControlBoards);

    return ret;
}


ReturnValue ControlBoardRemappingEngine::getRefPositions(const int n_joints, const int *joints, double *targets)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    // Resize the input buffers
    selectedJointsBuffers.resizeSubControlBoardBuffers(n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;

        if( p->posDir )
        {
            ok = p->posDir->getRefPositions(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                            selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                            selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }

        ret = ret && ok;
    }

    selectedJointsBuffers.fillArbitraryJointVectorFromSubControlBoardBuffers(targets,n_joints,joints,remappedControlBoards);

    return ret;
}



// IVelocityControl interface
ReturnValue ControlBoardRemappingEngine::velocityMove(const int n_joints, const int *joints, const double *spds)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(spds,n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;
        if (p->vel) {
            ok = p->vel->velocityMove(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                        selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                        selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getTargetVelocity(const int j, double* vel)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (p->vel)
    {
        ReturnValue ret = p->vel->getTargetVelocity(off, vel);
        return ret;
    }

    return ReturnValue::return_code::return_value_error_generic;
}


ReturnValue ControlBoardRemappingEngine::getTargetVelocities(double* vels)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;

        if( p->vel )
        {
            ok = p->vel->getTargetVelocities(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                           allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                           allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }

        ret = ret && ok;
    }

    allJointsBuffers.fillCompleteJointVectorFromSubControlBoardBuffers(vels,remappedControlBoards);

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getTargetVelocities(const int n_joints, const int* joints, double* vels)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    // Resize the input buffers
    selectedJointsBuffers.resizeSubControlBoardBuffers(n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;

        if( p->vel )
        {
            ok = p->vel->getTargetVelocities(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                           selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                           selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }

        ret = ret && ok;
    }

    selectedJointsBuffers.fillArbitraryJointVectorFromSubControlBoardBuffers(vels,n_joints,joints,remappedControlBoards);

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getInteractionMode(int j, yarp::dev::InteractionModeEnum& mode)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *s=remappedControlBoards.getSubControlBoard(subIndex);

    if (!s)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (s->iInteract)
    {
        return s->iInteract->getInteractionMode(off, mode);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::getInteractionModes(const std::vector<int>& joints, std::vector<yarp::dev::InteractionModeEnum>& modes)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    // Resize the input buffers
    selectedJointsBuffers.resizeSubControlBoardBuffers(joints.size(),joints.data(),remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;

        if( p->iMode )
        {
            ok = p->iInteract->getInteractionModes(selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd],
                                                   selectedJointsBuffers.m_bufferForSubControlBoardInteractionModes[ctrlBrd]);
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }

        ret = ret && ok;
    }

    selectedJointsBuffers.fillArbitraryJointVectorFromSubControlBoardBuffers(modes.data(), joints.size(), joints.data(), remappedControlBoards);

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getInteractionModes(std::vector<yarp::dev::InteractionModeEnum>& modes)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = ReturnValue_ok;

        if( p->iMode )
        {
            ok = p->iInteract->getInteractionModes(allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd],
                                                   allJointsBuffers.m_bufferForSubControlBoardInteractionModes[ctrlBrd]);
        }
        else
        {
            ok = ReturnValue::return_code::return_value_error_generic;
        }

        ret = ret && ok;
    }

    allJointsBuffers.fillCompleteJointVectorFromSubControlBoardBuffers(modes.data(), remappedControlBoards);

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setInteractionMode(int j, yarp::dev::InteractionModeEnum mode)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *s=remappedControlBoards.getSubControlBoard(subIndex);

    if (!s)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }

    if (s->iInteract)
    {
        return s->iInteract->setInteractionMode(off, mode);
    }

    return ReturnValue::return_code::return_value_error_generic;
}

ReturnValue ControlBoardRemappingEngine::setInteractionModes(const std::vector<int>& joints, const std::vector<yarp::dev::InteractionModeEnum>& modes)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(modes.data(), joints.size(), joints.data(), remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = p->iInteract->setInteractionModes(selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd],
                                                           selectedJointsBuffers.m_bufferForSubControlBoardInteractionModes[ctrlBrd]);
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setInteractionModes(const std::vector<yarp::dev::InteractionModeEnum>& modes)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(modes.data(),remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = p->iInteract->setInteractionModes(
                                                    allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd],
                                                    allJointsBuffers.m_bufferForSubControlBoardInteractionModes[ctrlBrd]);
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setRefDutyCycle(int m, double ref)
{
    ReturnValue ret = ReturnValue::return_code::return_value_error_generic;

    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iPwm)
    {
        int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
        ret = p->iPwm->setRefDutyCycle(off, ref);
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setRefDutyCycles(const double* refs)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iPwm)
        {
            ReturnValue ok = p->iPwm->setRefDutyCycle(off, refs[l]);
            ret = ret && ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getRefDutyCycle(int m, double* ref)
{
    ReturnValue ret = ReturnValue::return_code::return_value_error_generic;;

    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iPwm)
    {
        int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
        ret = p->iPwm->getRefDutyCycle(off, ref);
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getRefDutyCycles(double* refs)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iPwm)
        {
            ReturnValue ok = p->iPwm->getRefDutyCycle(off, refs+l);
            ret = ret && ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getDutyCycle(int m, double* val)
{
    ReturnValue ret = ReturnValue::return_code::return_value_error_generic;

    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iPwm)
    {
        int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
        ret = p->iPwm->getDutyCycle(off, val);
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getDutyCycles(double* vals)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iPwm)
        {
            ReturnValue ok = p->iPwm->getDutyCycle(off, vals+l);
            ret = ret && ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getCurrent(int j, double *val)
{
    ReturnValue ret = ReturnValue::return_code::return_value_error_generic;

    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iCurr)
    {
        int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
        ret = p->iCurr->getCurrent(off, val);
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getCurrents(double *vals)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iCurr)
        {
            ReturnValue ok = p->iCurr->getCurrent(off, vals+l);
            ret = ret && ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getCurrentRange(int m, double* min, double* max)
{
    ReturnValue ret = ReturnValue::return_code::return_value_error_generic;

    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iCurr)
    {
        int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
        ret = p->iCurr->getCurrentRange(off, min, max);
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getCurrentRanges(double* min, double* max)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iCurr)
        {
            ReturnValue ok = p->iCurr->getCurrentRange(off, min+l, max+l);
            ret = ret && ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setRefCurrent(int m, double curr)
{
    ReturnValue ret = ReturnValue::return_code::return_value_error_generic;

    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iCurr)
    {
        int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
        ret = p->iCurr->setRefCurrent(off, curr);
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setRefCurrents(const int n_motor, const int* motors, const double* currs)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(currs,n_motor,motors,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        if (!(p && p->iCurr))
        {
            ret = ReturnValue::return_code::return_value_error_generic;
            break;
        }

        ReturnValue ok = p->iCurr->setRefCurrents(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                          selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                          selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setRefCurrents(const double* currs)
{
    ReturnValue ret=ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(currs,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        ReturnValue ok = p->iCurr->setRefCurrents(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                           allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                           allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getRefCurrent(int m, double* curr)
{
    ReturnValue ret = ReturnValue::return_code::return_value_error_generic;

    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iCurr)
    {
        int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
        ret = p->iCurr->getRefCurrent(off, curr);
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getRefCurrents(double* currs)
{
    ReturnValue ret=ReturnValue_ok;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (p->iCurr)
        {
            ReturnValue ok = p->iCurr->getRefCurrent(off, currs+l);
            ret = ret && ok;
        }
        else
        {
            ret=ReturnValue::return_code::return_value_error_generic;
        }
    }

    return ret;
}

// IJointBraked interface


ReturnValue ControlBoardRemappingEngine::isJointBraked(int j, bool& braked) const
{
    ReturnValue ret = ReturnValue::return_code::return_value_error_not_ready;

    size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;
    RemappedSubControlBoard* p = remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iBrake) {
        int off = (int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
        ret = p->iBrake->isJointBraked(off, braked);
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setManualBrakeActive(int j, bool active)
{
    ReturnValue ret = ReturnValue::return_code::return_value_error_not_ready;

    size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;
    RemappedSubControlBoard* p = remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iBrake) {
        int off = (int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
        ret = p->iBrake->setManualBrakeActive(off, active);
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::setAutoBrakeEnabled(int j, bool enabled)
{
    ReturnValue ret = ReturnValue::return_code::return_value_error_not_ready;

    size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;
    RemappedSubControlBoard* p = remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iBrake) {
        int off = (int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
        ret = p->iBrake->setAutoBrakeEnabled(off, enabled);
    }

    return ret;
}

ReturnValue ControlBoardRemappingEngine::getAutoBrakeEnabled(int j, bool& enabled) const
{
    ReturnValue ret = ReturnValue::return_code::return_value_error_not_ready;

    size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;
    RemappedSubControlBoard* p = remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iBrake) {
        int off = (int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
        ret = p->iBrake->getAutoBrakeEnabled(off, enabled);
    }

    return ret;
}

// IVelocityDirect interface

yarp::dev::ReturnValue ControlBoardRemappingEngine::setRefVelocity(int jnt, double vel)
{
    int off = static_cast<int>(remappedControlBoards.lut[jnt].axisIndexInSubControlBoard);
    size_t subIndex = remappedControlBoards.lut[jnt].subControlBoardIndex;
    RemappedSubControlBoard* p = remappedControlBoards.getSubControlBoard(subIndex);

    if (!p || !p->iVelDir)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }
    return p->iVelDir->setRefVelocity(off, vel);
}

yarp::dev::ReturnValue ControlBoardRemappingEngine::setRefVelocity(const std::vector<double>& vels)
{
    if (vels.size() != static_cast<size_t>(controlledJoints))
    {
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    ReturnValue ret = ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);
    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(vels.data(), remappedControlBoards);

    for (size_t ctrlBrd = 0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard* p = remappedControlBoards.getSubControlBoard(ctrlBrd);
        if (!p || !p->iVelDir)
        {
            ret = ReturnValue::return_code::return_value_error_generic;
            continue;
        }
        std::vector<double> subVels = allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd];
        ret = ret && p->iVelDir->setRefVelocity(subVels);
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRemappingEngine::setRefVelocity(const std::vector<int>& jnts, const std::vector<double>& vels)
{
    if (jnts.size() != vels.size())
    {
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    ReturnValue ret = ReturnValue_ok;
    std::lock_guard<std::mutex> lock(selectedJointsBuffers.mutex);
    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(vels.data(), static_cast<int>(jnts.size()), jnts.data(), remappedControlBoards);

    for (size_t ctrlBrd = 0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        RemappedSubControlBoard* p = remappedControlBoards.getSubControlBoard(ctrlBrd);
        if (!p || !p->iVelDir)
        {
            ret = ReturnValue::return_code::return_value_error_generic;
            continue;
        }
        std::vector<int> subJnts = selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd];
        std::vector<double> subVels = selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd];
        ret = ret && p->iVelDir->setRefVelocity(subJnts, subVels);
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRemappingEngine::getRefVelocity(const int jnt, double& vel)
{
    int off = static_cast<int>(remappedControlBoards.lut[jnt].axisIndexInSubControlBoard);
    size_t subIndex = remappedControlBoards.lut[jnt].subControlBoardIndex;
    RemappedSubControlBoard* p = remappedControlBoards.getSubControlBoard(subIndex);

    if (!p || !p->iVelDir)
    {
        return ReturnValue::return_code::return_value_error_generic;
    }
    return p->iVelDir->getRefVelocity(off, vel);
}

yarp::dev::ReturnValue ControlBoardRemappingEngine::getRefVelocity(std::vector<double>& vels)
{
    vels.resize(controlledJoints);
    ReturnValue ret = ReturnValue_ok;
    std::lock_guard<std::mutex> lock(allJointsBuffers.mutex);

    for (int l = 0; l < controlledJoints; l++)
    {
        int off = static_cast<int>(remappedControlBoards.lut[l].axisIndexInSubControlBoard);
        size_t subIndex = remappedControlBoards.lut[l].subControlBoardIndex;
        RemappedSubControlBoard* p = remappedControlBoards.getSubControlBoard(subIndex);
        if (!p || !p->iVelDir)
        {
            ret = ReturnValue::return_code::return_value_error_generic;
            continue;
        }
        ret = ret && p->iVelDir->getRefVelocity(off, vels[l]);
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRemappingEngine::getRefVelocity(const std::vector<int>& jnts, std::vector<double>& vels)
{
    vels.resize(jnts.size());
    ReturnValue ret = ReturnValue_ok;

    for (size_t i = 0; i < jnts.size(); ++i)
    {
        int j = jnts[i];
        int off = static_cast<int>(remappedControlBoards.lut[j].axisIndexInSubControlBoard);
        size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;
        RemappedSubControlBoard* p = remappedControlBoards.getSubControlBoard(subIndex);
        if (!p || !p->iVelDir)
        {
            ret = ReturnValue::return_code::return_value_error_generic;
            continue;
        }
        ret = ret && p->iVelDir->getRefVelocity(off, vels[i]);
    }
    return ret;
}
