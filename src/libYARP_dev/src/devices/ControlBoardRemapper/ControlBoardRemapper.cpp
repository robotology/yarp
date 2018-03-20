/*
 * Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
 * Author: Lorenzo Natale, Silvio Traversaro
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "ControlBoardRemapper.h"
#include "ControlBoardRemapperHelpers.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <cassert>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace std;

// needed for the driver factory.
DriverCreator *createControlBoardRemapper()
{
    return new DriverCreatorOf<yarp::dev::ControlBoardRemapper>
            ("controlboardremapper", "controlboardwrapper2", "yarp::dev::ControlBoardRemapper");
}

ControlBoardRemapper::ControlBoardRemapper() :
    controlledJoints(0),
    _verb(false),
    usingAxesNamesForAttachAll(false),
    usingNetworksForAttachAll(false)
{
    axesNames.clear();
}

ControlBoardRemapper::~ControlBoardRemapper()
{
}

bool ControlBoardRemapper::close()
{
    return detachAll();
}

bool ControlBoardRemapper::open(Searchable& config)
{
    Property prop;
    prop.fromString(config.toString().c_str());

    _verb = (prop.check("verbose","if present, give detailed output"));
    if (_verb)
    {
        yInfo("ControlBoardRemapper: running with verbose output\n");
    }

    if(!parseOptions(prop))
    {
        return false;
    }

    return true;
}

bool ControlBoardRemapper::parseOptions(Property& prop)
{
    bool ok = true;

    usingAxesNamesForAttachAll  = prop.check("axesNames", "list of networks merged by this wrapper");
    usingNetworksForAttachAll = prop.check("networks", "list of networks merged by this wrapper");


    if( usingAxesNamesForAttachAll &&
        usingNetworksForAttachAll )
    {
        yError() << "controlBoardRemapper: Both axesNames and networks option present, this is not supported.\n";
        return false;
    }

    if( !usingAxesNamesForAttachAll &&
        !usingNetworksForAttachAll )
    {
        yError() << "controlBoardRemapper: axesNames option not found";
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

bool ControlBoardRemapper::parseAxesNames(const Property& prop)
{
    Bottle *propAxesNames=prop.find("axesNames").asList();
    if(propAxesNames==nullptr)
    {
       yError() <<"ControlBoardRemapper: Error parsing parameters: \"axesNames\" should be followed by a list\n";
       return false;
    }

    axesNames.resize(propAxesNames->size());
    for(int ax=0; ax < propAxesNames->size(); ax++)
    {
        axesNames[ax] = propAxesNames->get(ax).asString().c_str();
    }

    this->setNrOfControlledAxes(axesNames.size());

    return true;
}

bool ControlBoardRemapper::parseNetworks(const Property& prop)
{
    Bottle *nets=prop.find("networks").asList();
    if(nets==nullptr)
    {
       yError() <<"ControlBoardRemapper: Error parsing parameters: \"networks\" should be followed by a list\n";
       return false;
    }

    if (!prop.check("joints", "number of joints of the part"))
    {
        yError() <<"ControlBoardRemapper: joints options not found when reading networks option";
        return false;
    }

    this->setNrOfControlledAxes((size_t)prop.find("joints").asInt());

    int nsubdevices=nets->size();
    remappedControlBoards.lut.resize(controlledJoints);
    remappedControlBoards.subdevices.resize(nsubdevices);

    // configure the devices
    for(int k=0;k<nets->size();k++)
    {
        Bottle parameters;
        int wBase;
        int wTop;
        int base;
        int top;

        parameters=prop.findGroup(nets->get(k).asString().c_str());

        if(parameters.size()==2)
        {
            Bottle *bot=parameters.get(1).asList();
            Bottle tmpBot;
            if(bot==nullptr)
            {
                // probably data are not passed in the correct way, try to read them as a string.
                ConstString bString(parameters.get(1).asString());
                tmpBot.fromString(bString);

                if(tmpBot.size() != 4)
                {
                    yError() << "Error: check network parameters in part description"
                             << "--> I was expecting "<<nets->get(k).asString().c_str() << " followed by a list of four integers in parenthesis"
                             << "Got: "<< parameters.toString().c_str() << "\n";
                    return false;
                }
                else
                {
                    bot = &tmpBot;
                }
            }

            // If I came here, bot is correct
            wBase=bot->get(0).asInt();
            wTop=bot->get(1).asInt();
            base=bot->get(2).asInt();
            top=bot->get(3).asInt();
        }
        else if (parameters.size()==5)
        {
            // yError<<"Parameter networks use deprecated syntax\n";
            wBase=parameters.get(1).asInt();
            wTop=parameters.get(2).asInt();
            base=parameters.get(3).asInt();
            top=parameters.get(4).asInt();
        }
        else
        {
            yError() <<"Error: check network parameters in part description"
                     <<"--> I was expecting "<<nets->get(k).asString().c_str() << " followed by a list of four integers in parenthesis"
                     <<"Got: "<< parameters.toString().c_str() << "\n";
            return false;
        }

        RemappedSubControlBoard *tmpDevice=remappedControlBoards.getSubControlBoard((size_t)k);
        tmpDevice->setVerbose(_verb);

        if( (wTop-wBase) != (top-base) )
        {
            yError() <<"Error: check network parameters in network "<<nets->get(k).asString().c_str() <<
             "I was expecting a well form quadruple of numbers, got instead: "<< parameters.toString().c_str();
        }

        tmpDevice->id = nets->get(k).asString().c_str();

        for(int j=wBase;j<=wTop;j++)
        {
            int off = j-wBase;
            remappedControlBoards.lut[j].subControlBoardIndex=k;
            remappedControlBoards.lut[j].axisIndexInSubControlBoard=base+off;
        }
    }

    return true;
}

void ControlBoardRemapper::setNrOfControlledAxes(const size_t nrOfControlledAxes)
{
    controlledJoints = nrOfControlledAxes;
    buffers.controlBoardModes.resize(nrOfControlledAxes,0);
    buffers.dummyBuffer.resize(nrOfControlledAxes,0.0);
}


bool ControlBoardRemapper::updateAxesName()
{
    bool ret = true;
    axesNames.resize(controlledJoints);

    for(int l=0; l < controlledJoints; l++)
    {
        yarp::os::ConstString axNameYARP;
        bool ok = this->getAxisName(l,axNameYARP);
        if( ok )
        {
            axesNames[l] = axNameYARP.c_str();
        }

        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::attachAll(const PolyDriverList &polylist)
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
    for(unsigned int k=0; k<remappedControlBoards.getNrOfSubControlBoards(); k++)
    {
        if (!remappedControlBoards.subdevices[k].isAttached())
        {
            ready=false;
        }
    }

    if (!ready)
    {
        yError("ControlBoardRemapper: AttachAll failed, some subdevice was not found or its attach failed\n");
        return false;
    }

    if( ok )
    {
        configureBuffers();
    }

    return ok;
}

// First we store a map between each axes name
// in the passed PolyDriverList and the device in which they belong and their index
struct axisLocation
{
    ConstString subDeviceKey;
    size_t indexOfSubDeviceInPolyDriverList;
    int indexInSubDevice;
};


bool ControlBoardRemapper::attachAllUsingAxesNames(const PolyDriverList& polylist)
{
    std::map<std::string, axisLocation> axesLocationMap;

    for(int p=0;p<polylist.size();p++)
    {
        // If there is a device with a specific device key, use it
        // as a calibrator, otherwise rely on the subcontrolboards
        // as usual
        std::string deviceKey=polylist[p]->key.c_str();
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
            yError() <<"ControlBoardRemapper: subdevice " << deviceKey << " does not implemented the required IAxisInfo or IEncoders interfaces";
            return false;
        }

        int nrOfSubdeviceAxes;
        bool ok = iencs->getAxes(&nrOfSubdeviceAxes);

        if( !ok )
        {
            yError() <<"ControlBoardRemapper: subdevice " << deviceKey << " does not implemented the required getAxes method";
            return false;
        }

        for(int axInSubDevice =0; axInSubDevice < nrOfSubdeviceAxes; axInSubDevice++)
        {
            yarp::os::ConstString axNameYARP;
            ok = iaxinfos->getAxisName(axInSubDevice,axNameYARP);

            std::string axName = axNameYARP.c_str();

            if( !ok )
            {
                yError() <<"ControlBoardRemapper: subdevice " << deviceKey << " does not implemented the required getAxisName method";
                return false;
            }

            std::map<std::string, axisLocation>::iterator it = axesLocationMap.find(axName);
            if( it != axesLocationMap.end() )
            {
                yError() <<"ControlBoardRemapper: multiple axes have the same name " << axName
                         <<" on on device " << polylist[p]->key << " with index  " << axInSubDevice
                         <<" and another on device " << it->second.subDeviceKey << " with index " << it->second.indexInSubDevice;
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
    for(size_t l=0; l < axesNames.size(); l++)
    {
        std::map<std::string, axisLocation>::iterator it = axesLocationMap.find(axesNames[l]);
        if( it == axesLocationMap.end() )
        {
            yError() <<"ControlBoardRemapper: axis " << axesNames[l]
                     <<" specified in axesNames was not found in the axes of the controlboards "
                     <<"passed to attachAll, attachAll failed. ";
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


bool ControlBoardRemapper::attachAllUsingNetworks(const PolyDriverList &polylist)
{
    for(int p=0;p<polylist.size();p++)
    {
        // look if we have to attach to a calibrator
        std::string subDeviceKey = polylist[p]->key.c_str();
        if(subDeviceKey == "Calibrator" || subDeviceKey == "calibrator")
        {
            // Set the IRemoteCalibrator interface, the wrapper must point to the calibrator rdevice
            yarp::dev::IRemoteCalibrator *calibrator;
            polylist[p]->poly->view(calibrator);

            IRemoteCalibrator::setCalibratorDevice(calibrator);
            continue;
        }

        // find appropriate entry in list of subdevices and attach
        unsigned int k=0;
        for(k=0; k<remappedControlBoards.getNrOfSubControlBoards(); k++)
        {
            if (remappedControlBoards.subdevices[k].id==subDeviceKey)
            {
                if (!remappedControlBoards.subdevices[k].attach(polylist[p]->poly, subDeviceKey))
                {
                    yError("ControlBoardRemapper: attach to subdevice %s failed\n", polylist[p]->key.c_str());
                    return false;
                }
            }
        }
    }

    bool ok = updateAxesName();

    if( !ok )
    {
        yWarning() << "ControlBoardRemapper: unable to update axesNames";
    }

    return true;
}


bool ControlBoardRemapper::detachAll()
{
    //check if we already instantiated a subdevice previously
    int devices=remappedControlBoards.getNrOfSubControlBoards();
    for(int k=0;k<devices;k++)
        remappedControlBoards.getSubControlBoard(k)->detach();

    IRemoteCalibrator::releaseCalibratorDevice();
    return true;
}

void ControlBoardRemapper::configureBuffers()
{
    allJointsBuffers.configure(remappedControlBoards);
    selectedJointsBuffers.configure(remappedControlBoards);
}


bool ControlBoardRemapper::setControlModeAllAxes(const int cm)
{
    LockGuard guard(buffers.mutex);

    for(int j=0; j < controlledJoints; j++)
    {
        buffers.controlBoardModes[j] = cm;
    }

    return this->setControlModes(buffers.controlBoardModes.data());
}

//////////////////////////////////////////////////////////////////////////////
/// ControlBoard methods
//////////////////////////////////////////////////////////////////////////////

//
//  IPid Interface
//
bool ControlBoardRemapper::setPid(const PidControlTypeEnum& pidtype, int j, const Pid &p)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *s=remappedControlBoards.getSubControlBoard(subIndex);
    if (!s)
    {
        return false;
    }

    if (s->pid)
    {
        return s->pid->setPid(pidtype, off, p);
    }

    return false;
}

bool ControlBoardRemapper::setPids(const PidControlTypeEnum& pidtype, const Pid *ps)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;

        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return false;
        }

        if (p->pid)
        {
            bool ok = p->pid->setPid(pidtype, off, ps[l]);
            ret=ret&&ok;
        }
        else
        {
            ret=false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::setPidReference(const PidControlTypeEnum& pidtype, int j, double ref)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->pid)
    {
        return p->pid->setPidReference(pidtype, off, ref);
    }

    return false;
}

bool ControlBoardRemapper::setPidReferences(const PidControlTypeEnum& pidtype, const double *refs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;

        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return false;
        }

        if (p->pid)
        {
            bool ok = p->pid->setPidReference(pidtype, off, refs[l]);
            ret=ret&&ok;
        }
        else
        {
            ret=false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::setPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double limit)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
    if (!p)
    {
        return false;
    }

    if (p->pid)
    {
        return p->pid->setPidErrorLimit(pidtype, off, limit);
    }

    return false;
}

bool ControlBoardRemapper::setPidErrorLimits(const PidControlTypeEnum& pidtype, const double *limits)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;

        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return false;
        }

        if (p->pid)
        {
            bool ok = p->pid->setPidErrorLimit(pidtype, off, limits[l]);
            ret=ret&&ok;
        }
        else
        {
            ret=false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::getPidError(const PidControlTypeEnum& pidtype, int j, double *err)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->pid)
    {
        return p->pid->getPidError(pidtype, off, err);
    }

    return false;
}

bool ControlBoardRemapper::getPidErrors(const PidControlTypeEnum& pidtype, double *errs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return false;
        }

        if (p->pid)
        {
            bool ok = p->pid->getPidError(pidtype, off, errs+l);
            ret=ret&&ok;
        }
        else
        {
            ret=false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::getPidOutput(const PidControlTypeEnum& pidtype, int j, double *out)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
    if (!p)
    {
        return false;
    }

    if (p->pid)
    {
        return p->pid->getPidOutput(pidtype, off, out);
    }

    return false;
}

bool ControlBoardRemapper::getPidOutputs(const PidControlTypeEnum& pidtype, double *outs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;

        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return false;
        }

        if (p->pid)
        {
            ret=ret&&p->pid->getPidOutput(pidtype, off, outs+l);
        }
        else
        {
            ret=false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::setPidOffset(const PidControlTypeEnum& pidtype, int j, double v)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
    if (!p)
    {
        return false;
    }

    if (p->pid)
    {
        return p->pid->setPidOffset(pidtype, off, v);
    }

    return false;
}

bool ControlBoardRemapper::getPid(const PidControlTypeEnum& pidtype, int j, Pid *p)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *s=remappedControlBoards.getSubControlBoard(subIndex);
    if (!s)
    {
        return false;
    }

    if (s->pid)
    {
        return s->pid->getPid(pidtype, off, p);
    }

    return false;
}

bool ControlBoardRemapper::getPids(const PidControlTypeEnum& pidtype, Pid *pids)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return false;
        }

        if (p->pid)
        {
            bool ok = p->pid->getPid(pidtype, off, pids+l);
            ret = ok && ret;
        }
        else
        {
            ret=false;
        }
    }

    return ret;
}

bool ControlBoardRemapper::getPidReference(const PidControlTypeEnum& pidtype, int j, double *ref)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
    if (!p)
    {
        return false;
    }

    if (p->pid)
    {
        return p->pid->getPidReference(pidtype, off, ref);
    }

    return false;
}

bool ControlBoardRemapper::getPidReferences(const PidControlTypeEnum& pidtype, double *refs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;

        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return false;
        }

        if (p->pid)
        {
            bool ok = p->pid->getPidReference(pidtype, off, refs+l);
            ret=ret && ok;
        }
        else
        {
            ret=false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double *limit)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->pid)
    {
        return p->pid->getPidErrorLimit(pidtype, off, limit);
    }
    return false;
}

bool ControlBoardRemapper::getPidErrorLimits(const PidControlTypeEnum& pidtype, double *limits)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;

        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->pid)
        {
            bool ok = p->pid->getPidErrorLimit(pidtype, off, limits+l);
            ret=ret&&ok;
        }
        else
        {
            ret=false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::resetPid(const PidControlTypeEnum& pidtype, int j)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->pid)
    {
        return p->pid->resetPid(pidtype, off);
    }

    return false;
}

bool ControlBoardRemapper::disablePid(const PidControlTypeEnum& pidtype, int j)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    return p->pid->disablePid(pidtype, off);
}

bool ControlBoardRemapper::enablePid(const PidControlTypeEnum& pidtype, int j)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->pid)
    {
        return p->pid->enablePid(pidtype, off);
    }

    return false;
}

bool ControlBoardRemapper::isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool* enabled)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->pid)
    {
        return p->pid->isPidEnabled(pidtype, off, enabled);
    }

    return false;
}

/* IPositionControl */
bool ControlBoardRemapper::getAxes(int *ax)
{
    *ax=controlledJoints;
    return true;
}

bool ControlBoardRemapper::positionMove(int j, double ref)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->pos2)
    {
        return p->pos2->positionMove(off, ref);
    }

    return false;
}

bool ControlBoardRemapper::positionMove(const double *refs)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(refs,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->pos2->positionMove(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                        allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                        allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::positionMove(const int n_joints, const int *joints, const double *refs)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(refs,n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->pos2->positionMove(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                        selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                        selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::getTargetPosition(const int j, double* ref)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->pos2)
    {
        bool ret = p->pos2->getTargetPosition(off, ref);
        return ret;
    }

    return false;
}

bool ControlBoardRemapper::getTargetPositions(double *spds)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = true;

        if( p->pos2 )
        {
            ok = p->pos2->getTargetPositions(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                             allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                             allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = false;
        }

        ret = ret && ok;
    }

    allJointsBuffers.fillCompleteJointVectorFromSubControlBoardBuffers(spds,remappedControlBoards);

    return ret;
}


bool ControlBoardRemapper::getTargetPositions(const int n_joints, const int *joints, double *targets)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    // Resize the input buffers
    selectedJointsBuffers.resizeSubControlBoardBuffers(n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = true;

        if( p->pos2 )
        {
            ok = p->pos2->getTargetPositions(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                             selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                             selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = false;
        }

        ret = ret && ok;
    }

    selectedJointsBuffers.fillArbitraryJointVectorFromSubControlBoardBuffers(targets,n_joints,joints,remappedControlBoards);

    return ret;
}

bool ControlBoardRemapper::relativeMove(int j, double delta)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->pos2)
    {
        return p->pos2->relativeMove(off, delta);
    }

    return false;
}

bool ControlBoardRemapper::relativeMove(const double *deltas)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(deltas,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->pos2->relativeMove(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                        allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                        allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::relativeMove(const int n_joints, const int *joints, const double *deltas)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(deltas,n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->pos2->relativeMove(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                        selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                        selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::checkMotionDone(int j, bool *flag)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->pos2)
    {
        return p->pos2->checkMotionDone(off, flag);
    }

    return false;
}

bool ControlBoardRemapper::checkMotionDone(bool *flag)
{
    bool ret=true;
    *flag=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->pos2)
        {
            bool singleJointMotionDone =false;
            bool ok = p->pos2->checkMotionDone(off, &singleJointMotionDone);
            ret = ret && ok;
            *flag = *flag  && singleJointMotionDone;
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::checkMotionDone(const int n_joints, const int *joints, bool *flag)
{
    bool ret=true;
    *flag=true;

    for(int j=0;j<n_joints;j++)
    {
        int l = joints[j];

        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->pos2)
        {
            bool singleJointMotionDone =false;
            bool ok = p->pos2->checkMotionDone(off, &singleJointMotionDone);
            ret = ret && ok;
            *flag = *flag  && singleJointMotionDone;
        }
        else
        {
            ret = false;
        }
    }

    return ret;
}

bool ControlBoardRemapper::setRefSpeed(int j, double sp)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->pos2)
    {
        return p->pos2->setRefSpeed(off, sp);
    }

    return false;
}

bool ControlBoardRemapper::setRefSpeeds(const double *spds)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(spds,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->pos2->setRefSpeeds(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                        allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                        allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::setRefSpeeds(const int n_joints, const int *joints, const double *spds)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(spds,n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->pos2->setRefSpeeds(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                        selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                        selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::setRefAcceleration(int j, double acc)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->pos2)
    {
        return p->pos2->setRefAcceleration(off, acc);
    }

    return false;
}

bool ControlBoardRemapper::setRefAccelerations(const double *accs)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(accs,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->pos2->setRefAccelerations(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                               allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                               allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::setRefAccelerations(const int n_joints, const int *joints, const double *accs)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(accs,n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->pos2->setRefAccelerations(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                               selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                               selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::getRefSpeed(int j, double *ref)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->pos2)
    {
        return p->pos2->getRefSpeed(off, ref);
    }

    return false;
}

bool ControlBoardRemapper::getRefSpeeds(double *spds)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = true;

        if( p->pos2 )
        {
            ok = p->pos2->getRefSpeeds(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                       allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                       allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = false;
        }

        ret = ret && ok;
    }

    allJointsBuffers.fillCompleteJointVectorFromSubControlBoardBuffers(spds,remappedControlBoards);

    return ret;
}


bool ControlBoardRemapper::getRefSpeeds(const int n_joints, const int *joints, double *spds)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    // Resize the input buffers
    selectedJointsBuffers.resizeSubControlBoardBuffers(n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = true;

        if( p->pos2 )
        {
            ok = p->pos2->getRefSpeeds(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                            selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                            selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = false;
        }

        ret = ret && ok;
    }

    selectedJointsBuffers.fillArbitraryJointVectorFromSubControlBoardBuffers(spds,n_joints,joints,remappedControlBoards);

    return ret;
}

bool ControlBoardRemapper::getRefAcceleration(int j, double *acc)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->pos2)
    {
        return p->pos2->getRefAcceleration(off, acc);
    }

    return false;
}

bool ControlBoardRemapper::getRefAccelerations(double *accs)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = true;

        if( p->pos2 )
        {
            ok = p->pos2->getRefAccelerations(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                              allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                              allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = false;
        }

        ret = ret && ok;
    }

    allJointsBuffers.fillCompleteJointVectorFromSubControlBoardBuffers(accs,remappedControlBoards);

    return ret;
}

bool ControlBoardRemapper::getRefAccelerations(const int n_joints, const int *joints, double *accs)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    // Resize the input buffers
    selectedJointsBuffers.resizeSubControlBoardBuffers(n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = true;

        if( p->pos2 )
        {
            ok = p->pos2->getRefAccelerations(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                              selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                              selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = false;
        }

        ret = ret && ok;
    }

    selectedJointsBuffers.fillArbitraryJointVectorFromSubControlBoardBuffers(accs,n_joints,joints,remappedControlBoards);

    return ret;
}

bool ControlBoardRemapper::stop(int j)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->pos2)
    {
        return p->pos2->stop(off);
    }

    return false;
}

bool ControlBoardRemapper::stop()
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = true;

        ok = p->pos2 ? p->pos2->stop(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                     allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data()) : false;

        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::stop(const int n_joints, const int *joints)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);
    yarp::os::LockGuard(buffers.mutex);


    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(buffers.dummyBuffer.data(),n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->pos2->stop(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}


/* IVelocityControl */

bool ControlBoardRemapper::velocityMove(int j, double v)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->vel2)
    {
        return p->vel2->velocityMove(off, v);
    }

    return false;
}

bool ControlBoardRemapper::velocityMove(const double *v)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(v,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->vel2->velocityMove(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                        allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                        allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

/* IEncoders */
bool ControlBoardRemapper::resetEncoder(int j)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
    if (!p)
    {
        return false;
    }

    if (p->iJntEnc)
    {
        return p->iJntEnc->resetEncoder(off);
    }

    return false;
}

bool ControlBoardRemapper::resetEncoders()
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return false;
        }

        if (p->iJntEnc)
        {
            bool ok = p->iJntEnc->resetEncoder(off);
            ret = ret && ok;
        }
        else
        {
            ret=false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::setEncoder(int j, double val)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iJntEnc)
    {
        return p->iJntEnc->setEncoder(off,val);
    }

    return false;
}

bool ControlBoardRemapper::setEncoders(const double *vals)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off = (int) remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex = remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iJntEnc)
        {
            bool ok = p->iJntEnc->setEncoder(off, vals[l]);
            ret = ret && ok;
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::getEncoder(int j, double *v)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iJntEnc)
    {
        return p->iJntEnc->getEncoder(off, v);
    }

    return false;
}

bool ControlBoardRemapper::getEncoders(double *encs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iJntEnc)
        {
            bool ok = p->iJntEnc->getEncoder(off, encs+l);
            ret = ret && ok;
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::getEncodersTimed(double *encs, double *t)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iJntEnc)
        {
            bool ok = p->iJntEnc->getEncoderTimed(off, encs+l, t+l);
            ret = ret && ok;
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::getEncoderTimed(int j, double *v, double *t)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iJntEnc)
    {
        return p->iJntEnc->getEncoderTimed(off, v, t);
    }

    return false;
}

bool ControlBoardRemapper::getEncoderSpeed(int j, double *sp)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iJntEnc)
    {
        return p->iJntEnc->getEncoderSpeed(off, sp);
    }

    return false;
}

bool ControlBoardRemapper::getEncoderSpeeds(double *spds)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iJntEnc)
        {
            bool ok = p->iJntEnc->getEncoderSpeed(off, spds+l);
            ret = ret && ok;
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::getEncoderAcceleration(int j, double *acc)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iJntEnc)
    {
        return p->iJntEnc->getEncoderAcceleration(off,acc);
    }

    return false;
}

bool ControlBoardRemapper::getEncoderAccelerations(double *accs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iJntEnc)
        {
            bool ok = p->iJntEnc->getEncoderAcceleration(off, accs+l);
            ret = ret && ok;
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}

/* IMotor */
bool ControlBoardRemapper::getNumberOfMotors(int *num)
{
    *num=controlledJoints;
    return true;
}

bool ControlBoardRemapper::getTemperature(int m, double* val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->imotor)
    {
        return p->imotor->getTemperature(off, val);
    }

    return false;
}

bool ControlBoardRemapper::getTemperatures(double *vals)
{
    bool ret=true;
    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return false;
        }

        if (p->imotor)
        {
            ret=ret&&p->imotor->getTemperature(off, vals+l);
        }
        else
        {
            ret=false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::getTemperatureLimit(int m, double* val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->imotor)
    {
        return p->imotor->getTemperatureLimit(off, val);
    }

    return false;
}

bool ControlBoardRemapper::setTemperatureLimit (int m, const double val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->imotor)
    {
        return p->imotor->setTemperatureLimit(off,val);
    }

    return false;
}

bool ControlBoardRemapper::getGearboxRatio(int m, double* val)
{
    int off = (int)remappedControlBoards.lut[m].subControlBoardIndex;
    size_t subIndex = remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->imotor)
    {
        return p->imotor->getGearboxRatio(off, val);
    }

    return false;
}

bool ControlBoardRemapper::setGearboxRatio(int m, const double val)
{
    int off = (int)remappedControlBoards.lut[m].subControlBoardIndex;
    size_t subIndex = remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->imotor)
    {
        return p->imotor->setGearboxRatio(off, val);
    }

    return false;
}

/* IRemoteVariables */
bool ControlBoardRemapper::getRemoteVariable(yarp::os::ConstString key, yarp::os::Bottle& val)
{
    yWarning("ControlBoardRemapper: getRemoteVariable is not properly implemented, use at your own risk.");

    bool b = true;

    for (unsigned int i = 0; i < remappedControlBoards.getNrOfSubControlBoards(); i++)
    {
        yarp::dev::RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(i);

        if (!p)
        {
            return false;
        }

        if (!p->iVar)
        {
            return false;
        }

        yarp::os::Bottle tmpval;
        bool ok = p->iVar->getRemoteVariable(key, tmpval);

        if (ok)
        {
            val.append(tmpval);
        }

        b = b && ok;
    }

    return b;
}

bool ControlBoardRemapper::setRemoteVariable(yarp::os::ConstString key, const yarp::os::Bottle& val)
{
    yWarning("ControlBoardRemapper: setRemoteVariable is not properly implemented, use at your own risk.");

    size_t bottle_size = val.size();
    size_t device_size = remappedControlBoards.getNrOfSubControlBoards();
    if (bottle_size != device_size)
    {
        yError("ControlBoardRemapper::setRemoteVariable bottle_size != device_size failure");
        return false;
    }

    bool b = true;
    for (unsigned int i = 0; i < device_size; i++)
    {
        yarp::dev::RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(i);
        if (!p)  { yError("ControlBoardRemapper::setRemoteVariable !p failure"); return false; }
        if (!p->iVar) { yError("ControlBoardRemapper::setRemoteVariable !p->iVar failure"); return false; }
        Bottle* partial_val = val.get(i).asList();
        if (partial_val)
        {
            b &= p->iVar->setRemoteVariable(key, *partial_val);
        }
        else
        {
            yError("ControlBoardRemapper::setRemoteVariable general failure");
            return false;
        }
    }

    return b;
}

bool ControlBoardRemapper::getRemoteVariablesList(yarp::os::Bottle* listOfKeys)
{
    yWarning("ControlBoardRemapper: getRemoteVariablesList is not properly implemented, use at your own risk.");

    size_t subIndex = remappedControlBoards.lut[0].subControlBoardIndex;
    yarp::dev::RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iVar)
    {
        return p->iVar->getRemoteVariablesList(listOfKeys);
    }

    return false;
}

/* IMotorEncoders */

bool ControlBoardRemapper::resetMotorEncoder(int m)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iMotEnc)
    {
        return p->iMotEnc->resetMotorEncoder(off);
    }

    return false;
}

bool ControlBoardRemapper::resetMotorEncoders()
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
        {
            return false;
        }

        if (p->iMotEnc)
        {
            bool ok = p->iMotEnc->resetMotorEncoder(off);
            ret= ret && ok;
        }
        else
        {
            ret=false;
        }
    }

    return ret;
}

bool ControlBoardRemapper::setMotorEncoder(int m, const double val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iMotEnc)
    {
        return p->iMotEnc->setMotorEncoder(off,val);
    }

    return false;
}

bool ControlBoardRemapper::setMotorEncoders(const double *vals)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iMotEnc)
        {
            bool ok = p->iMotEnc->setMotorEncoder(off, vals[l]);
            ret = ret && ok;
        }
        else
        {
            ret=false;
        }
    }

    return ret;
}

bool ControlBoardRemapper::setMotorEncoderCountsPerRevolution(int m, double cpr)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iMotEnc)
    {
        return p->iMotEnc->setMotorEncoderCountsPerRevolution(off,cpr);
    }

    return false;
}

bool ControlBoardRemapper::getMotorEncoderCountsPerRevolution(int m, double *cpr)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoderCountsPerRevolution(off, cpr);
    }

    return false;
}

bool ControlBoardRemapper::getMotorEncoder(int m, double *v)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoder(off, v);
    }

    return false;
}

bool ControlBoardRemapper::getMotorEncoders(double *encs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
        if (!p)
            return false;

        if (p->iMotEnc)
        {
            ret=ret&&p->iMotEnc->getMotorEncoder(off, encs+l);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardRemapper::getMotorEncodersTimed(double *encs, double *t)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iMotEnc)
        {
            bool ok = p->iMotEnc->getMotorEncoderTimed(off, encs, t);
            ret = ret && ok;
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::getMotorEncoderTimed(int m, double *v, double *t)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoderTimed(off, v, t);
    }

    return false;
}

bool ControlBoardRemapper::getMotorEncoderSpeed(int m, double *sp)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoderSpeed(off, sp);
    }

    return false;
}

bool ControlBoardRemapper::getMotorEncoderSpeeds(double *spds)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off = (int) remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex = remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iMotEnc)
        {
            bool ok = p->iMotEnc->getMotorEncoderSpeed(off, spds + l);
            ret = ret && ok;
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::getMotorEncoderAcceleration(int m, double *acc)
{
    int off = (int) remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex = remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoderAcceleration(off,acc);
    }
    *acc=0.0;
    return false;
}

bool ControlBoardRemapper::getMotorEncoderAccelerations(double *accs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iMotEnc)
        {
            bool ok = p->iMotEnc->getMotorEncoderAcceleration(off, accs+l);
            ret=ret && ok;
        }
        else
        {
            ret=false;
        }
    }

    return ret;
}


bool ControlBoardRemapper::getNumberOfMotorEncoders(int *num)
{
    *num=controlledJoints;
    return true;
}

/* IAmplifierControl */
bool ControlBoardRemapper::enableAmp(int j)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->amp)
    {
        return p->amp->enableAmp(off);
    }

    return false;
}

bool ControlBoardRemapper::disableAmp(int j)
{
    return this->setControlMode(j, VOCAB_CM_IDLE);
}

bool ControlBoardRemapper::getAmpStatus(int *st)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->amp)
        {
            bool ok = p->amp->getAmpStatus(off, st+l);
            ret = ret && ok;
        }
        else
        {
            ret=false;
        }
    }

    return ret;
}

bool ControlBoardRemapper::getAmpStatus(int j, int *v)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->amp)
    {
        return p->amp->getAmpStatus(off,v);
    }

    return false;
}

bool ControlBoardRemapper::setMaxCurrent(int j, double v)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->amp)
    {
        return p->amp->setMaxCurrent(off,v);
    }

    return false;
}

bool ControlBoardRemapper::getMaxCurrent(int j, double* v)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->amp)
    {
        return p->amp->getMaxCurrent(off,v);
    }

    return false;
}

bool ControlBoardRemapper::getNominalCurrent(int m, double *val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if(!p)
    {
        return false;
    }

    if(!p->amp)
    {
        return false;
    }

    return p->amp->getNominalCurrent(off, val);
}

bool ControlBoardRemapper::getPeakCurrent(int m, double *val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if(!p)
    {
        return false;
    }

    if(!p->amp)
    {
        return false;
    }

    return p->amp->getPeakCurrent(off, val);
}

bool ControlBoardRemapper::setPeakCurrent(int m, const double val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (!p->amp)
    {
        return false;
    }

    return p->amp->setPeakCurrent(off, val);
}

bool ControlBoardRemapper::getPWM(int m, double* val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if(!p)
    {
        return false;
    }

    if(!p->amp)
    {
        return false;
    }

    return p->amp->getPWM(off, val);
}
bool ControlBoardRemapper::getPWMLimit(int m, double* val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if(!p)
    {
        return false;
    }

    if(!p->amp)
    {
        return false;
    }

    return p->amp->getPWMLimit(off, val);
}

bool ControlBoardRemapper::setPWMLimit(int m, const double val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (!p->amp)
    {
        return false;
    }

    return p->amp->setPWMLimit(off, val);
}

bool ControlBoardRemapper::getPowerSupplyVoltage(int m, double* val)
{
    int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if(!p)
    {
        return false;
    }

    if(!p->amp)
    {
        return false;
    }

    return p->amp->getPowerSupplyVoltage(off, val);
}


/* IControlLimits */

bool ControlBoardRemapper::setLimits(int j, double min, double max)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->lim2)
    {
        return p->lim2->setLimits(off,min, max);
    }

    return false;
}

bool ControlBoardRemapper::getLimits(int j, double *min, double *max)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->lim2)
    {
        return p->lim2->getLimits(off,min, max);
    }

    return false;
}

bool ControlBoardRemapper::setVelLimits(int j, double min, double max)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (!p->lim2)
    {
        return false;
    }

    return p->lim2->setVelLimits(off,min, max);
}

bool ControlBoardRemapper::getVelLimits(int j, double *min, double *max)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if(!p->lim2)
    {
        return false;
    }

    return p->lim2->getVelLimits(off,min, max);
}

/* IRemoteCalibrator */
IRemoteCalibrator *ControlBoardRemapper::getCalibratorDevice()
{
    return yarp::dev::IRemoteCalibrator::getCalibratorDevice();
}

bool ControlBoardRemapper::isCalibratorDevicePresent(bool *isCalib)
{
    return yarp::dev::IRemoteCalibrator::isCalibratorDevicePresent(isCalib);
}

bool ControlBoardRemapper::calibrateSingleJoint(int j)
{
    if(!getCalibratorDevice())
    {
        int off = (int) remappedControlBoards.lut[j].axisIndexInSubControlBoard;
        size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *s = remappedControlBoards.getSubControlBoard(subIndex);
        if (!s)
        {
            return false;
        }

        if (s->remcalib)
        {
            return s->remcalib->calibrateSingleJoint(off);
        }

        return false;
    }
    else
    {
        return IRemoteCalibrator::getCalibratorDevice()->calibrateSingleJoint(j);
    }
}

bool ControlBoardRemapper::calibrateWholePart()
{
    if(!getCalibratorDevice())
    {
        for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
        {
            yarp::dev::RemappedSubControlBoard *s = remappedControlBoards.getSubControlBoard(ctrlBrd);
            if (!s)
            {
                return false;
            }

            if (s->remcalib)
            {
                return s->remcalib->calibrateWholePart();
            }
        }

        return false;
    }
    else
    {
        return IRemoteCalibrator::getCalibratorDevice()->calibrateWholePart();
    }
}

bool ControlBoardRemapper::homingSingleJoint(int j)
{
    if(!getCalibratorDevice())
    {
        int off = (int) remappedControlBoards.lut[j].axisIndexInSubControlBoard;
        size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *s = remappedControlBoards.getSubControlBoard(subIndex);
        if (!s)
        {
            return false;
        }

        if (s->remcalib)
        {
            return s->remcalib->homingSingleJoint(off);
        }

        return false;
    }
    else
    {
        return IRemoteCalibrator::getCalibratorDevice()->homingSingleJoint(j);
    }
}

bool ControlBoardRemapper::homingWholePart()
{
    if(!getCalibratorDevice())
    {
        bool ret = true;
        for(int l=0;l<controlledJoints;l++)
        {
            bool ok = this->homingSingleJoint(l);
            ret = ret && ok;
        }

        return ret;
    }
    else
    {
        return IRemoteCalibrator::getCalibratorDevice()->homingWholePart();
    }
}

bool ControlBoardRemapper::parkSingleJoint(int j, bool _wait)
{
    if(!getCalibratorDevice())
    {
        int off = (int) remappedControlBoards.lut[j].axisIndexInSubControlBoard;
        size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *s = remappedControlBoards.getSubControlBoard(subIndex);
        if (!s)
        {
            return false;
        }

        if (s->remcalib)
        {
            return s->remcalib->parkSingleJoint(off,_wait);
        }

        return false;
    }
    else
    {
        return getCalibratorDevice()->parkSingleJoint(j, _wait);
    }
}

bool ControlBoardRemapper::parkWholePart()
{
    if(!getCalibratorDevice())
    {
        bool ret = true;

        for(int l=0; l<controlledJoints; l++)
        {
            bool _wait = false;
            bool ok = this->parkSingleJoint(l,_wait);
            ret = ret && ok;
        }

        return ret;
    }
    else
    {
        return getCalibratorDevice()->parkWholePart();
    }
}

bool ControlBoardRemapper::quitCalibrate()
{
    if(!getCalibratorDevice())
    {
        for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
        {
            yarp::dev::RemappedSubControlBoard *s = remappedControlBoards.getSubControlBoard(ctrlBrd);
            if (!s)
            {
                return false;
            }

            if (s->remcalib)
            {
                return s->remcalib->quitCalibrate();
            }
        }

        return false;
    }
    else
    {
        return IRemoteCalibrator::getCalibratorDevice()->quitCalibrate();
    }
}

bool ControlBoardRemapper::quitPark()
{
    if(!getCalibratorDevice())
    {
        for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
        {
            yarp::dev::RemappedSubControlBoard *s = remappedControlBoards.getSubControlBoard(ctrlBrd);
            if (!s)
            {
                return false;
            }

            if (s->remcalib)
            {
                return s->remcalib->quitPark();
            }
        }

        return false;
    }
    else
    {
        return IRemoteCalibrator::getCalibratorDevice()->quitPark();
    }
}


/* IControlCalibration */

bool ControlBoardRemapper::calibrate(int j, double p)
{
    int off = (int) remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *s = remappedControlBoards.getSubControlBoard(subIndex);
    if (!s)
    {
        return false;
    }

    if (s->calib)
    {
        return s->calib->calibrate(off, p);
    }

    return false;
}

bool ControlBoardRemapper::calibrate2(int j, unsigned int ui, double v1, double v2, double v3)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->calib2)
    {
        return p->calib2->calibrate2(off, ui,v1,v2,v3);
    }
    return false;
}

bool ControlBoardRemapper::setCalibrationParameters(int j, const CalibrationParameters& params)
{
    int off = (int) remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->calib2)
    {
        return p->calib2->setCalibrationParameters(off, params);
    }

    return false;
}

bool ControlBoardRemapper::done(int j)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->calib2)
    {
        return p->calib2->done(off);
    }

    return false;
}

bool ControlBoardRemapper::abortPark()
{
    yError("ControlBoardRemapper: Calling abortPark -- not implemented\n");
    return false;
}

bool ControlBoardRemapper::abortCalibration()
{
    yError("ControlBoardRemapper: Calling abortCalibration -- not implemented\n");
    return false;
}

/* IAxisInfo */

bool ControlBoardRemapper::getAxisName(int j, yarp::os::ConstString& name)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->info)
    {
        return p->info->getAxisName(off, name);
    }

    return false;
}

bool ControlBoardRemapper::getJointType(int j, yarp::dev::JointTypeEnum& type)
{
    int off = (int) remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->info)
    {
        return p->info->getJointType(off, type);
    }

    return false;
}

bool ControlBoardRemapper::getRefTorques(double *refs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iTorque)
        {
            bool ok = p->iTorque->getRefTorque(off, refs+l);
            ret = ret && ok;
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}

bool ControlBoardRemapper::getRefTorque(int j, double *t)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iTorque)
    {
        return p->iTorque->getRefTorque(off, t);
    }
    return false;
}

bool ControlBoardRemapper::setRefTorques(const double *t)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(t,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok;

        if( p->iTorque )
        {
            ok = p->iTorque->setRefTorques(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                           allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                           allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = false;
        }

        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::setRefTorque(int j, double t)
{
    int off = (int) remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex = remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p = remappedControlBoards.getSubControlBoard(subIndex);
    if (!p)
    {
        return false;
    }

    if (p->iTorque)
    {
        return p->iTorque->setRefTorque(off, t);
    }
    return false;
}

bool ControlBoardRemapper::setRefTorques(const int n_joints, const int *joints, const double *t)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(t,n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->iTorque->setRefTorques(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                            selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                            selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}


bool ControlBoardRemapper::getBemfParam(int j, double *t)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->getBemfParam(off, t);
    }
    return false;
}

bool ControlBoardRemapper::setBemfParam(int j, double t)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iTorque)
    {
        return p->iTorque->setBemfParam(off, t);
    }

    return false;
}

bool ControlBoardRemapper::getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iTorque)
    {
        return p->iTorque->getMotorTorqueParams(off, params);
    }

    return false;
}

bool ControlBoardRemapper::setMotorTorqueParams(int j,  const yarp::dev::MotorTorqueParameters params)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iTorque)
    {
        return p->iTorque->setMotorTorqueParams(off, params);
    }

    return false;
}

bool ControlBoardRemapper::setImpedance(int j, double stiff, double damp)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iImpedance)
    {
        return p->iImpedance->setImpedance(off, stiff, damp);
    }

    return false;
}

bool ControlBoardRemapper::setImpedanceOffset(int j, double offset)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iImpedance)
    {
        return p->iImpedance->setImpedanceOffset(off, offset);
    }

    return false;
}

bool ControlBoardRemapper::getTorque(int j, double *t)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iTorque)
    {
        return p->iTorque->getTorque(off, t);
    }

    return false;
}

bool ControlBoardRemapper::getTorques(double *t)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iTorque)
        {
            bool ok = p->iTorque->getTorque(off, t+l);
            ret = ret && ok;
        }
        else
        {
            ret=false;
        }
    }

    return ret;
 }

bool ControlBoardRemapper::getTorqueRange(int j, double *min, double *max)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iTorque)
    {
        return p->iTorque->getTorqueRange(off, min, max);
    }

    return false;
}

bool ControlBoardRemapper::getTorqueRanges(double *min, double *max)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iTorque)
        {
            bool ok = p->iTorque->getTorqueRange(off, min+l, max+l);
            ret = ret && ok;
        }
        else
        {
            ret=false;
        }
    }
    return ret;
 }

bool ControlBoardRemapper::getImpedance(int j, double* stiff, double* damp)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iImpedance)
    {
        return p->iImpedance->getImpedance(off, stiff, damp);
    }

    return false;
}

bool ControlBoardRemapper::getImpedanceOffset(int j, double* offset)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iImpedance)
    {
        return p->iImpedance->getImpedanceOffset(off, offset);
    }

    return false;
}

bool ControlBoardRemapper::getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->iImpedance)
    {
        return p->iImpedance->getCurrentImpedanceLimit(off, min_stiff, max_stiff, min_damp, max_damp);
    }

    return false;
}

bool ControlBoardRemapper::getControlMode(int j, int *mode)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);
    if (!p)
        return false;

    return p->iMode2->getControlMode(off, mode);

}

bool ControlBoardRemapper::getControlModes(int *modes)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok;

        if( p->iMode2 )
        {
            ok = p->iMode2->getControlModes(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                            allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                            allJointsBuffers.m_bufferForSubControlBoardControlModes[ctrlBrd].data());
        }
        else
        {
            ok = false;
        }

        ret = ret && ok;
    }

    allJointsBuffers.fillCompleteJointVectorFromSubControlBoardBuffers(modes,remappedControlBoards);

    return ret;
}

// iControlMode2
bool ControlBoardRemapper::getControlModes(const int n_joints, const int *joints, int *modes)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    // Resize the input buffers
    selectedJointsBuffers.resizeSubControlBoardBuffers(n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok;

        if( p->iMode2 )
        {
            ok = p->iMode2->getControlModes(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                            selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                            selectedJointsBuffers.m_bufferForSubControlBoardControlModes[ctrlBrd].data());
        }
        else
        {
            ok = false;
        }

        ret = ret && ok;
    }

    selectedJointsBuffers.fillArbitraryJointVectorFromSubControlBoardBuffers(modes,n_joints,joints,remappedControlBoards);

    return ret;
}

bool ControlBoardRemapper::setControlMode(const int j, const int mode)
{
    bool ret = true;

    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    ret = p->iMode2->setControlMode(off, mode);

    return ret;
}

bool ControlBoardRemapper::setControlModes(const int n_joints, const int *joints, int *modes)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(modes,n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->iMode2->setControlModes(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                             selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                             selectedJointsBuffers.m_bufferForSubControlBoardControlModes[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::setControlModes(int *modes)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(modes,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->iMode2->setControlModes(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                             allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                             allJointsBuffers.m_bufferForSubControlBoardControlModes[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::setPosition(int j, double ref)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->posDir)
    {
        return p->posDir->setPosition(off, ref);
    }

    return false;
}

bool ControlBoardRemapper::setPositions(const int n_joints, const int *joints, double *dpos)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(dpos,n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->posDir->setPositions(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                          selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                          selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::setPositions(const double *refs)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(refs,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->posDir->setPositions(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                          allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                          allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

yarp::os::Stamp ControlBoardRemapper::getLastInputStamp()
{
    double averageTimestamp = 0.0;
    int collectedTimestamps = 0;

    for(int l=0;l<controlledJoints;l++)
    {
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

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


    yarp::os::LockGuard(buffers.mutex);

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

bool ControlBoardRemapper::getRefPosition(const int j, double* ref)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->posDir)
    {
        bool ret = p->posDir->getRefPosition(off, ref);
        return ret;
    }

    return false;
}

bool ControlBoardRemapper::getRefPositions(double *spds)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = true;

        if( p->posDir )
        {
            ok = p->posDir->getRefPositions(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                            allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                            allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = false;
        }

        ret = ret && ok;
    }

    allJointsBuffers.fillCompleteJointVectorFromSubControlBoardBuffers(spds,remappedControlBoards);

    return ret;
}


bool ControlBoardRemapper::getRefPositions(const int n_joints, const int *joints, double *targets)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    // Resize the input buffers
    selectedJointsBuffers.resizeSubControlBoardBuffers(n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = true;

        if( p->posDir )
        {
            ok = p->posDir->getRefPositions(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                            selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                            selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = false;
        }

        ret = ret && ok;
    }

    selectedJointsBuffers.fillArbitraryJointVectorFromSubControlBoardBuffers(targets,n_joints,joints,remappedControlBoards);

    return ret;
}


//
// IVelocityControl2 Interface
//
bool ControlBoardRemapper::velocityMove(const int n_joints, const int *joints, const double *spds)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(spds,n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->vel2->velocityMove(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                        selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                        selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::getRefVelocity(const int j, double* vel)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (!p)
    {
        return false;
    }

    if (p->vel2)
    {
        bool ret = p->vel2->getRefVelocity(off, vel);
        return ret;
    }

    return false;
}


bool ControlBoardRemapper::getRefVelocities(double* vels)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = true;

        if( p->vel2 )
        {
            ok = p->vel2->getRefVelocities(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                           allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                           allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = false;
        }

        ret = ret && ok;
    }

    allJointsBuffers.fillCompleteJointVectorFromSubControlBoardBuffers(vels,remappedControlBoards);

    return ret;
}

bool ControlBoardRemapper::getRefVelocities(const int n_joints, const int* joints, double* vels)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    // Resize the input buffers
    selectedJointsBuffers.resizeSubControlBoardBuffers(n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = true;

        if( p->vel2 )
        {
            ok = p->vel2->getRefVelocities(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                           selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                           selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        }
        else
        {
            ok = false;
        }

        ret = ret && ok;
    }

    selectedJointsBuffers.fillArbitraryJointVectorFromSubControlBoardBuffers(vels,n_joints,joints,remappedControlBoards);

    return ret;
}

bool ControlBoardRemapper::getInteractionMode(int j, yarp::dev::InteractionModeEnum* mode)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *s=remappedControlBoards.getSubControlBoard(subIndex);

    if (!s)
    {
        return false;
    }

    if (s->iInteract)
    {
        return s->iInteract->getInteractionMode(off, mode);
    }

    return false;
}

bool ControlBoardRemapper::getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    // Resize the input buffers
    selectedJointsBuffers.resizeSubControlBoardBuffers(n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = true;

        if( p->iMode2 )
        {
            ok = p->iInteract->getInteractionModes(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                                   selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                                   selectedJointsBuffers.m_bufferForSubControlBoardInteractionModes[ctrlBrd].data());
        }
        else
        {
            ok = false;
        }

        ret = ret && ok;
    }

    selectedJointsBuffers.fillArbitraryJointVectorFromSubControlBoardBuffers(modes,n_joints,joints,remappedControlBoards);

    return ret;
}

bool ControlBoardRemapper::getInteractionModes(yarp::dev::InteractionModeEnum* modes)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = true;

        if( p->iMode2 )
        {
            ok = p->iInteract->getInteractionModes(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                                   allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                                   allJointsBuffers.m_bufferForSubControlBoardInteractionModes[ctrlBrd].data());
        }
        else
        {
            ok = false;
        }

        ret = ret && ok;
    }

    allJointsBuffers.fillCompleteJointVectorFromSubControlBoardBuffers(modes,remappedControlBoards);

    return ret;
}

bool ControlBoardRemapper::setInteractionMode(int j, yarp::dev::InteractionModeEnum mode)
{
    int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *s=remappedControlBoards.getSubControlBoard(subIndex);

    if (!s)
    {
        return false;
    }

    if (s->iInteract)
    {
        return s->iInteract->setInteractionMode(off, mode);
    }

    return false;
}

bool ControlBoardRemapper::setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(modes,n_joints,joints,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->iInteract->setInteractionModes(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                                    selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                                    selectedJointsBuffers.m_bufferForSubControlBoardInteractionModes[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::setInteractionModes(yarp::dev::InteractionModeEnum* modes)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(modes,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->iInteract->setInteractionModes(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                                    allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                                    allJointsBuffers.m_bufferForSubControlBoardInteractionModes[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::setRefDutyCycle(int m, double ref)
{
    bool ret = false;

    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iPwm)
    {
        int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
        ret = p->iPwm->setRefDutyCycle(off, ref);
    }

    return ret;
}

bool ControlBoardRemapper::setRefDutyCycles(const double* refs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iPwm)
        {
            bool ok = p->iPwm->setRefDutyCycle(off, refs[l]);
            ret = ret && ok;
        }
        else
        {
            ret=false;
        }
    }

    return ret;
}

bool ControlBoardRemapper::getRefDutyCycle(int m, double* ref)
{
    bool ret = false;

    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iPwm)
    {
        int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
        ret = p->iPwm->getRefDutyCycle(off, ref);
    }

    return ret;
}

bool ControlBoardRemapper::getRefDutyCycles(double* refs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iPwm)
        {
            bool ok = p->iPwm->getRefDutyCycle(off, refs+l);
            ret = ret && ok;
        }
        else
        {
            ret=false;
        }
    }

    return ret;
}

bool ControlBoardRemapper::getDutyCycle(int m, double* val)
{
    bool ret = false;

    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iPwm)
    {
        int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
        ret = p->iPwm->getDutyCycle(off, val);
    }

    return ret;
}

bool ControlBoardRemapper::getDutyCycles(double* vals)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iPwm)
        {
            bool ok = p->iPwm->getDutyCycle(off, vals+l);
            ret = ret && ok;
        }
        else
        {
            ret=false;
        }
    }

    return ret;
}

bool ControlBoardRemapper::getCurrent(int j, double *val)
{
    bool ret = false;

    size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iCurr)
    {
        int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
        ret = p->iCurr->getCurrent(off, val);
    }

    return ret;
}

bool ControlBoardRemapper::getCurrents(double *vals)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iCurr)
        {
            bool ok = p->iCurr->getCurrent(off, vals+l);
            ret = ret && ok;
        }
        else
        {
            ret=false;
        }
    }

    return ret;
}

bool ControlBoardRemapper::getCurrentRange(int m, double* min, double* max)
{
    bool ret = false;

    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iCurr)
    {
        int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
        ret = p->iCurr->getCurrentRange(off, min, max);
    }

    return ret;
}

bool ControlBoardRemapper::getCurrentRanges(double* min, double* max)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iCurr)
        {
            bool ok = p->iCurr->getCurrentRange(off, min+l, max+l);
            ret = ret && ok;
        }
        else
        {
            ret=false;
        }
    }

    return ret;
}

bool ControlBoardRemapper::setRefCurrent(int m, double curr)
{
    bool ret = false;

    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iCurr)
    {
        int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
        ret = p->iCurr->setRefCurrent(off, curr);
    }

    return ret;
}

bool ControlBoardRemapper::setRefCurrents(const int n_motor, const int* motors, const double* currs)
{
    bool ret=true;
    yarp::os::LockGuard(selectedJointsBuffers.mutex);

    selectedJointsBuffers.fillSubControlBoardBuffersFromArbitraryJointVector(currs,n_motor,motors,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        if (!(p && p->iCurr))
        {
            ret = false;
            break;
        }

        bool ok = p->iCurr->setRefCurrents(selectedJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                          selectedJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                          selectedJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::setRefCurrents(const double* currs)
{
    bool ret=true;
    yarp::os::LockGuard(allJointsBuffers.mutex);

    allJointsBuffers.fillSubControlBoardBuffersFromCompleteJointVector(currs,remappedControlBoards);

    for(size_t ctrlBrd=0; ctrlBrd < remappedControlBoards.getNrOfSubControlBoards(); ctrlBrd++)
    {
        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(ctrlBrd);

        bool ok = p->iCurr->setRefCurrents(allJointsBuffers.m_nJointsInSubControlBoard[ctrlBrd],
                                           allJointsBuffers.m_jointsInSubControlBoard[ctrlBrd].data(),
                                           allJointsBuffers.m_bufferForSubControlBoard[ctrlBrd].data());
        ret = ret && ok;
    }

    return ret;
}

bool ControlBoardRemapper::getRefCurrent(int m, double* curr)
{
    bool ret = false;

    size_t subIndex=remappedControlBoards.lut[m].subControlBoardIndex;

    yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

    if (p && p->iCurr)
    {
        int off=(int)remappedControlBoards.lut[m].axisIndexInSubControlBoard;
        ret = p->iCurr->getRefCurrent(off, curr);
    }

    return ret;
}

bool ControlBoardRemapper::getRefCurrents(double* currs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=(int)remappedControlBoards.lut[l].axisIndexInSubControlBoard;
        size_t subIndex=remappedControlBoards.lut[l].subControlBoardIndex;

        yarp::dev::RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

        if (!p)
        {
            return false;
        }

        if (p->iCurr)
        {
            bool ok = p->iCurr->getRefCurrent(off, currs+l);
            ret = ret && ok;
        }
        else
        {
            ret=false;
        }
    }

    return ret;
}


