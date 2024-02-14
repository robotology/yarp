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
    Property prop;
    prop.fromString(config.toString());

    _verb = (prop.check("verbose","if present, give detailed output"));
    if (_verb)
    {
        yCInfo(CONTROLBOARDCOUPLINGHANDLER, "running with verbose output");
    }

    if(!parseOptions(prop))
    {
        return false;
    }

    return true;
}

bool ControlBoardCouplingHandler::parseOptions(Property& prop)
{
    bool ok = true;

    usingAxesNamesForAttachAll  = prop.check("axesNames", "list of networks merged by this wrapper");
    usingNetworksForAttachAll = prop.check("networks", "list of networks merged by this wrapper");


    if( usingAxesNamesForAttachAll &&
        usingNetworksForAttachAll )
    {
        yCError(CONTROLBOARDCOUPLINGHANDLER) << "Both axesNames and networks option present, this is not supported.";
        return false;
    }

    if( !usingAxesNamesForAttachAll &&
        !usingNetworksForAttachAll )
    {
        yCError(CONTROLBOARDCOUPLINGHANDLER) << "axesNames option not found";
        return false;
    }

    if( usingAxesNamesForAttachAll )
    {
        ok = parseAxesNames(prop);
    }

    if( usingNetworksForAttachAll )
    {
        ok = parseNetworks(prop);
    }

    return ok;
}

bool ControlBoardCouplingHandler::parseAxesNames(const Property& prop)
{
    Bottle *propAxesNames=prop.find("axesNames").asList();
    if(propAxesNames==nullptr)
    {
       yCError(CONTROLBOARDCOUPLINGHANDLER) << "Parsing parameters: \"axesNames\" should be followed by a list";
       return false;
    }

    axesNames.resize(propAxesNames->size());
    for(size_t ax=0; ax < propAxesNames->size(); ax++)
    {
        axesNames[ax] = propAxesNames->get(ax).asString();
    }

    this->setNrOfControlledAxes(axesNames.size());

    return true;
}

bool ControlBoardCouplingHandler::parseNetworks(const Property& prop)
{
    Bottle *nets=prop.find("networks").asList();
    if(nets==nullptr)
    {
       yCError(CONTROLBOARDCOUPLINGHANDLER) << "Parsing parameters: \"networks\" should be followed by a list";
       return false;
    }

    if (!prop.check("joints", "number of joints of the part"))
    {
        yCError(CONTROLBOARDCOUPLINGHANDLER) << "joints options not found when reading networks option";
        return false;
    }

    this->setNrOfControlledAxes((size_t)prop.find("joints").asInt32());

    int nsubdevices=nets->size();

    // configure the devices
    for(size_t k=0;k<nets->size();k++)
    {
        Bottle parameters;
        int wBase;
        int wTop;
        int base;
        int top;

        parameters=prop.findGroup(nets->get(k).asString());

        if(parameters.size()==2)
        {
            Bottle *bot=parameters.get(1).asList();
            Bottle tmpBot;
            if(bot==nullptr)
            {
                // probably data are not passed in the correct way, try to read them as a string.
                std::string bString(parameters.get(1).asString());
                tmpBot.fromString(bString);

                if(tmpBot.size() != 4)
                {
                    yCError(CONTROLBOARDCOUPLINGHANDLER)
                        << "Check network parameters in part description"
                        << "--> I was expecting" << nets->get(k).asString() << "followed by a list of four integers in parenthesis"
                        << "Got:" << parameters.toString();
                    return false;
                }
                else
                {
                    bot = &tmpBot;
                }
            }

            // If I came here, bot is correct
            wBase=bot->get(0).asInt32();
            wTop=bot->get(1).asInt32();
            base=bot->get(2).asInt32();
            top=bot->get(3).asInt32();
        }
        else if (parameters.size()==5)
        {
            // yCError(CONTROLBOARDCOUPLINGHANDLER) << "Parameter networks use deprecated syntax";
            wBase=parameters.get(1).asInt32();
            wTop=parameters.get(2).asInt32();
            base=parameters.get(3).asInt32();
            top=parameters.get(4).asInt32();
        }
        else
        {
            yCError(CONTROLBOARDCOUPLINGHANDLER)
                << "Check network parameters in part description"
                << "--> I was expecting" << nets->get(k).asString() << "followed by a list of four integers in parenthesis"
                << "Got:" << parameters.toString();
            return false;
        }

    }

    return true;
}

void ControlBoardCouplingHandler::setNrOfControlledAxes(const size_t nrOfControlledAxes)
{
    controlledJoints = nrOfControlledAxes;
}


bool ControlBoardCouplingHandler::updateAxesName()
{
    bool ret = true;
    axesNames.resize(controlledJoints);

    for(int l=0; l < controlledJoints; l++)
    {
        std::string axNameYARP;
        bool ok = this->getAxisName(l,axNameYARP);
        if( ok )
        {
            axesNames[l] = axNameYARP;
        }

        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardCouplingHandler::attachAll(const PolyDriverList &polylist)
{
    // For both cases, now configure everything that need
    // all the attribute to be correctly configured
    bool ok = false;

    if( usingAxesNamesForAttachAll )
    {
        ok = attachAllUsingAxesNames(polylist);
    }

    if( usingNetworksForAttachAll )
    {
        ok = attachAllUsingNetworks(polylist);
    }

    //check if all devices are attached to the driver
    bool ready=true;

    if (!ready)
    {
        yCError(CONTROLBOARDCOUPLINGHANDLER, "AttachAll failed, some subdevice was not found or its attach failed");
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
    for(const auto& axesName : axesNames)
    {
        auto it = axesLocationMap.find(axesName);
        if( it == axesLocationMap.end() )
        {
            yCError(CONTROLBOARDCOUPLINGHANDLER)
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

    return true;
}


bool ControlBoardCouplingHandler::attachAllUsingNetworks(const PolyDriverList &polylist)
{
    for(int p=0;p<polylist.size();p++)
    {
        // look if we have to attach to a calibrator
        std::string subDeviceKey = polylist[p]->key;
        if(subDeviceKey == "Calibrator" || subDeviceKey == "calibrator")
        {
            // Set the IRemoteCalibrator interface, the wrapper must point to the calibrator rdevice
            yarp::dev::IRemoteCalibrator *calibrator;
            polylist[p]->poly->view(calibrator);

            //IRemoteCalibrator::setCalibratorDevice(calibrator);
            continue;
        }
    }

    bool ok = updateAxesName();

    if( !ok )
    {
        yCWarning(CONTROLBOARDCOUPLINGHANDLER) << "unable to update axesNames";
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

bool ControlBoardCouplingHandler::getAxes(int *ax)
{
    *ax=controlledJoints;
    return true;
}

/* IEncoders */
bool ControlBoardCouplingHandler::resetEncoder(int j)
{

    return false;
}

bool ControlBoardCouplingHandler::resetEncoders()
{
    bool ret=true;
    return ret;
}

bool ControlBoardCouplingHandler::setEncoder(int j, double val)
{

    return false;
}

bool ControlBoardCouplingHandler::setEncoders(const double *vals)
{
    bool ret=true;
    return ret;
}

bool ControlBoardCouplingHandler::getEncoder(int j, double *v)
{
    return false;
}

bool ControlBoardCouplingHandler::getEncoders(double *encs)
{
    bool ret=true;
    return ret;
}

bool ControlBoardCouplingHandler::getEncodersTimed(double *encs, double *t)
{
    bool ret=true;
    return ret;
}

bool ControlBoardCouplingHandler::getEncoderTimed(int j, double *v, double *t)
{
    return false;
}

bool ControlBoardCouplingHandler::getEncoderSpeed(int j, double *sp)
{
    return false;
}

bool ControlBoardCouplingHandler::getEncoderSpeeds(double *spds)
{
    bool ret=true;
    return ret;
}

bool ControlBoardCouplingHandler::getEncoderAcceleration(int j, double *acc)
{

    return false;
}

bool ControlBoardCouplingHandler::getEncoderAccelerations(double *accs)
{
    bool ret=true;

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

bool ControlBoardCouplingHandler::getAxisName(int j, std::string& name)
{

    return false;
}

bool ControlBoardCouplingHandler::getJointType(int j, yarp::dev::JointTypeEnum& type)
{

    return false;
}

yarp::os::Stamp ControlBoardCouplingHandler::getLastInputStamp()
{
    double averageTimestamp = 0.0;
    int collectedTimestamps = 0;
    yarp::os::Stamp ret;

    return ret;
}
