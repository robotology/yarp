/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardCouplingHandler.h"
#include "ControlBoardCouplingHandlerLogComponent.h"

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

bool ControlBoardCouplingHandler::close()
{
    return detachAll();
}

bool ControlBoardCouplingHandler::open(Searchable& config)
{
    bool ret {false};

    if(!parseParams(config))
    {
        yCError(CONTROLBOARDCOUPLINGHANDLER) << "Error in parsing parameters";
        return false;
    }

    Property joint_coupling_config;
    joint_coupling_config.fromString(config.toString());
    joint_coupling_config.put("device", m_COUPLING_device);
    if(!jointCouplingHandler.open(config)) {
        yCError(CONTROLBOARDCOUPLINGHANDLER) << "Error in opening jointCouplingHandler device";
        return false;
    }

    if(!jointCouplingHandler.view(iJntCoupling)) {
        yCError(CONTROLBOARDCOUPLINGHANDLER) << "Error viewing the IJointCoupling interface";
        return false;
    }

    this->setNrOfControlledAxes(m_axesNames.size());

    Property prop;
    prop.fromString(config.toString());

    _verb = (prop.check("verbose","if present, give detailed output"));
    if (_verb)
    {
        yCInfo(CONTROLBOARDCOUPLINGHANDLER, "running with verbose output");
    }

    return true;
}

bool ControlBoardCouplingHandler::attachAll(const PolyDriverList &polylist)
{
    // For both cases, now configure everything that need
    // all the attribute to be correctly configured
    bool ok = attachAllUsingAxesNames(polylist);

    if (!ok)
    {
        yCError(CONTROLBOARDCOUPLINGHANDLER, "attachAll failed  some subdevice was not found or its attach failed");
        return false;
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


bool ControlBoardCouplingHandler::attachAllUsingAxesNames(const PolyDriverList& polylist)
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

            //IRemoteCalibrator::setCalibratorDevice(calibrator);
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
            yCError(CONTROLBOARDCOUPLINGHANDLER) << "sub-device" << deviceKey << "does not implemented the required IAxisInfo or IEncoders interfaces";
            return false;
        }

        int nrOfSubdeviceAxes;
        bool ok = iencs->getAxes(&nrOfSubdeviceAxes);

        if( !ok )
        {
            yCError(CONTROLBOARDCOUPLINGHANDLER) << "sub-device" << deviceKey << "does not implemented the required getAxes method";
            return false;
        }

        for(int axInSubDevice =0; axInSubDevice < nrOfSubdeviceAxes; axInSubDevice++)
        {
            std::string axNameYARP;
            ok = iaxinfos->getAxisName(axInSubDevice,axNameYARP);

            std::string axName = axNameYARP;

            if( !ok )
            {
                yCError(CONTROLBOARDCOUPLINGHANDLER) << "sub-device" << deviceKey << "does not implemented the required getAxisName method";
                return false;
            }

            auto it = axesLocationMap.find(axName);
            if( it != axesLocationMap.end() )
            {
                yCError(CONTROLBOARDCOUPLINGHANDLER)
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
    for(const auto& axesName : m_axesNames)
    {
        auto it = axesLocationMap.find(axesName);
        if( it == axesLocationMap.end() )
        {
            yCError(CONTROLBOARDCOUPLINGHANDLER)
                << "axis " << axesName
                << "specified in m_axesNames was not found in the axes of the controlboards passed to attachAll, attachAll failed.";
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

    return true;
}

bool ControlBoardCouplingHandler::detachAll()
{
    return true;
}

void ControlBoardCouplingHandler::configureBuffers()
{
}

//////////////////////////////////////////////////////////////////////////////
/// ControlBoard methods
//////////////////////////////////////////////////////////////////////////////


/* IEncoders */
bool ControlBoardCouplingHandler::resetEncoder(int j)
{
    return false;
}

bool ControlBoardCouplingHandler::resetEncoders()
{
    return false;
}

bool ControlBoardCouplingHandler::setEncoder(int j, double val)
{
    return false;
}

bool ControlBoardCouplingHandler::setEncoders(const double *vals)
{
    return false;
}

bool ControlBoardCouplingHandler::getEncoder(int j, double *v)
{
    // TODO use iJointCoupling
    return false;
}

bool ControlBoardCouplingHandler::getEncoders(double *encs)
{
    // TODO use iJointCoupling
    bool ret=true;
    return ret;
}

bool ControlBoardCouplingHandler::getEncodersTimed(double *encs, double *t)
{
    // TODO use iJointCoupling
    bool ret=true;
    return ret;
}

bool ControlBoardCouplingHandler::getEncoderTimed(int j, double *v, double *t)
{
    // TODO use iJointCoupling
    return false;
}

bool ControlBoardCouplingHandler::getEncoderSpeed(int j, double *sp)
{
    // TODO use iJointCoupling
    return false;
}

bool ControlBoardCouplingHandler::getEncoderSpeeds(double *spds)
{
    // TODO use iJointCoupling
    bool ret=true;
    return ret;
}

bool ControlBoardCouplingHandler::getEncoderAcceleration(int j, double *acc)
{
    // TODO use iJointCoupling
    return false;
}

bool ControlBoardCouplingHandler::getEncoderAccelerations(double *accs)
{
    bool ret=true;
    // TODO use iJointCoupling
    return ret;
}

// /* IControlLimits */

// bool ControlBoardCouplingHandler::setLimits(int j, double min, double max)
// {
//     int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
//     size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

//     RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

//     if (!p)
//     {
//         return false;
//     }

//     if (p->lim)
//     {
//         return p->lim->setLimits(off,min, max);
//     }

//     return false;
// }

// bool ControlBoardCouplingHandler::getLimits(int j, double *min, double *max)
// {
//     int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
//     size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

//     RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

//     if (!p)
//     {
//         return false;
//     }

//     if (p->lim)
//     {
//         return p->lim->getLimits(off,min, max);
//     }

//     return false;
// }

// bool ControlBoardCouplingHandler::setVelLimits(int j, double min, double max)
// {
//     int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
//     size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

//     RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

//     if (!p)
//     {
//         return false;
//     }

//     if (!p->lim)
//     {
//         return false;
//     }

//     return p->lim->setVelLimits(off,min, max);
// }

// bool ControlBoardCouplingHandler::getVelLimits(int j, double *min, double *max)
// {
//     int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
//     size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

//     RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

//     if (!p)
//     {
//         return false;
//     }

//     if(!p->lim)
//     {
//         return false;
//     }

//     return p->lim->getVelLimits(off,min, max);
// }

/* IAxisInfo */

bool ControlBoardCouplingHandler::getAxes(int *ax)
{
    // TODO use iJointCoupling
    bool ok{false};
    return ok;
}

bool ControlBoardCouplingHandler::getAxisName(int j, std::string& name)
{
    auto ok = iJntCoupling->getPhysicalJointName(j, name);
    return ok;
}

bool ControlBoardCouplingHandler::getJointType(int j, yarp::dev::JointTypeEnum& type)
{
    // TODO use iJointCoupling
    return false;
}

yarp::os::Stamp ControlBoardCouplingHandler::getLastInputStamp()
{
    double averageTimestamp = 0.0;
    int collectedTimestamps = 0;
    yarp::os::Stamp ret;

    return ret;
}
