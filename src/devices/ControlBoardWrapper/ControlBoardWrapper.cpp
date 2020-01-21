/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapper.h"
#include "StreamingMessagesParser.h"
#include "RPCMessagesParser.h"
#include <yarp/dev/impl/jointData.h>
#include <iostream>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <sstream>
#include <numeric>
#include <algorithm>

#include <cstring>         // for memset function

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::impl;
using namespace yarp::sig;
using namespace std;

ControlBoardWrapper::ControlBoardWrapper() :yarp::os::PeriodicThread(0.02),
                                            ownDevices(true)
{
    streaming_parser.init(this);
    RPC_parser.init(this);
    controlledJoints = 0;
    period = 0.02; // s.
    base = 0;
    top = 0;
    subDeviceOwned = nullptr;
    _verb = false;

    // init ROS data
    rosNodeName = "";
    rosTopicName = "";
    rosNode = nullptr;
    rosMsgCounter = 0;
    useROS = ROS_disabled;
    jointNames.clear();
}

void ControlBoardWrapper::cleanup_yarpPorts()
{
    //shut down control port
    inputRPCPort.interrupt();
    inputRPCPort.removeCallbackLock();
    inputRPCPort.close();

    inputStreamingPort.interrupt();
    inputStreamingPort.close();

    outputPositionStatePort.interrupt();
    outputPositionStatePort.close();

    extendedOutputStatePort.interrupt();
    extendedOutputStatePort.close();

    rpcData.destroy();
}

ControlBoardWrapper::~ControlBoardWrapper() = default;

bool ControlBoardWrapper::close()
{
    //stop thread if running
    detachAll();

    if (yarp::os::PeriodicThread::isRunning())
    {
        yarp::os::PeriodicThread::stop();
    }

    if(useROS != ROS_only)
    {
        cleanup_yarpPorts();
    }

    if(rosNode != nullptr)
    {
        delete rosNode;
        rosNode = nullptr;
    }

    //if we own a deviced we have to close and delete it
    if (ownDevices)
    {
        // we should have created a new devices which we need to delete
        if(subDeviceOwned != nullptr)
        {
            subDeviceOwned->close();
            delete subDeviceOwned;
            subDeviceOwned = nullptr;
        }
    }
    else
    {
        detachAll();
    }
    return true;
}

bool ControlBoardWrapper::checkPortName(Searchable &params)
{
    /* see if rootName is present in the config file, this param is not used from long time, so it'll be
     * marked as deprecated.
     */
    if(params.check("rootName"))
    {
        yWarning() <<   " ControlBoardWrapper2 device:\n"
                        "************************************************************************************\n"
                        "* ControlBoardWrapper2 is using the deprecated parameter 'rootName' for port name, *\n"
                        "* It has to be removed and substituted with:                                       *\n"
                        "*     name:    full port prefix name with leading '/',  e.g.  /robotName/part/     *\n"
                        "************************************************************************************";
        rootName = params.find("rootName").asString();
    }

    // find name as port name (similar both in new and old policy
    if(!params.check("name"))
    {
        yError() <<     " ControlBoardWrapper2 device:\n"
                        "************************************************************************************\n"
                        "* ControlBoardWrapper2 missing mandatory parameter 'name' for port name, usage is: *\n"
                        "*     name:    full port prefix name with leading '/',  e.g.  /robotName/part/     *\n"
                        "************************************************************************************";
        return false;
    }

    partName = params.find("name").asString();
    if(partName[0] != '/')
    {
        yWarning() <<   " ControlBoardWrapper2 device:\n"
                        "************************************************************************************\n"
                        "* ControlBoardWrapper2 'name' parameter for port name does not follow convention,  *\n"
                        "* it MUST start with a leading '/' since it is used as the full prefix port name   *\n"
                        "*     name:    full port prefix name with leading '/',  e.g.  /robotName/part/     *\n"
                        "* A temporary automatic fix will be done for you, but please fix your config file  *\n"
                        "************************************************************************************";
        rootName = "/" + partName;
    }
    else
    {
        rootName = partName;
    }

    return true;
}

bool ControlBoardWrapper::checkROSParams(Searchable &config)
{
    // check for ROS parameter group
    if(!config.check("ROS") )
    {
        useROS = ROS_disabled;
        return true;
    }

    yInfo()  << "ROS group was FOUND in config file.";

    Bottle &rosGroup = config.findGroup("ROS");
    if(rosGroup.isNull())
    {
        yError() << partName << "ROS group params is not a valid group or empty";
        useROS = ROS_config_error;
        return false;
    }

    // check for useROS parameter
    if(!rosGroup.check("useROS"))
    {
        yError() << partName << " cannot find useROS parameter, mandatory when using ROS message. \n \
                    Allowed values are true, false, ROS_only";
        useROS = ROS_config_error;
        return false;
    }
    std::string ros_use_type = rosGroup.find("useROS").asString();
    if(ros_use_type == "false")
    {
        yInfo() << partName << "useROS topic if set to 'false'";
        useROS = ROS_disabled;
        return true;
    }
    else if(ros_use_type == "true")
    {
        yInfo() << partName << "useROS topic if set to 'true'";
        useROS = ROS_enabled;
    }
    else if(ros_use_type == "only")
    {
        yInfo() << partName << "useROS topic if set to 'only";
        useROS = ROS_only;
    }
    else
    {
        yInfo() << partName << "useROS parameter is seet to unvalid value ('" << ros_use_type << "'), supported values are 'true', 'false', 'only'";
        useROS = ROS_config_error;
        return false;
    }

    // check for ROS_nodeName parameter
    if(!rosGroup.check("ROS_nodeName"))
    {
        yError() << partName << " cannot find ROS_nodeName parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    rosNodeName = rosGroup.find("ROS_nodeName").asString();  // TODO: check name is correct
    yInfo() << partName << "rosNodeName is " << rosNodeName;

    // check for ROS_topicName parameter
    if(!rosGroup.check("ROS_topicName"))
    {
        yError() << partName << " cannot find rosTopicName parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    rosTopicName = rosGroup.find("ROS_topicName").asString();
    yInfo() << partName << "rosTopicName is " << rosTopicName;

    // check for rosNodeName parameter
    // UPDATE: joint names are got from MotionControl subdevice now.
    // An error should be thrown later on in case we fail getting names from device
    if(!rosGroup.check("jointNames"))
    {
        yInfo() << partName << "ROS topic has been required, jointNames will be got from motionControl subdevice.";
    }
    else  // if names are there, store them. They will be used for back compatibility if old policy is used.
    {
        Bottle nameList = rosGroup.findGroup("jointNames").tail();
        if (nameList.isNull())
        {
            yError() << partName << " jointNames not found\n";
            useROS = ROS_config_error;
            return false;
        }

        if(nameList.size() != (size_t) controlledJoints)
        {
            yError() << partName << " jointNames incorrect number of entries. \n jointNames is " << nameList.toString() << "while expected length is " << controlledJoints;
            useROS = ROS_config_error;
            return false;
        }

        jointNames.clear();
        for(int i=0; i<controlledJoints; i++)
        {
            jointNames.push_back(nameList.get(i).toString());
        }
    }
    return true;
}

bool ControlBoardWrapper::initialize_ROS()
{
    bool success = false;
    switch(useROS)
    {
        case ROS_enabled:
        case ROS_only:
        {
            rosNode = new yarp::os::Node(rosNodeName);   // add a ROS node

            if(rosNode == nullptr)
            {
                yError() << " opening " << rosNodeName << " Node, check your yarp-ROS network configuration\n";
                success = false;
                break;
            }

            if (!rosPublisherPort.topic(rosTopicName) )
            {
                yError() << " opening " << rosTopicName << " Topic, check your yarp-ROS network configuration\n";
                success = false;
                break;
            }
            success = true;
        } break;

        case ROS_disabled:
        {
            yInfo() << partName << ": no ROS initialization required";
            success = true;
        } break;

        case ROS_config_error:
        {
            yError() << partName << " ROS parameter are not correct, check your configuration file";
            success = false;
        } break;

        default:
        {
            yError() << partName << " something went wrong with ROS configuration, we should never be here!!!";
            success = false;
        } break;
    }
    return success;
}

bool ControlBoardWrapper::initialize_YARP(yarp::os::Searchable &prop)
{
    bool success = false;

    switch(useROS)
    {
        case ROS_only:
        {
            yInfo() << partName << " No YARP initialization required";
            success = true;
        } break;

        default:
        {
            yInfo() << partName << " initting YARP initialization";
            // initialize callback
            if (!streaming_parser.initialize())
            {
                yError() <<"Error could not initialize callback object";
                success = false;
                break;
            }

            rootName = prop.check("rootName",Value("/"), "starting '/' if needed.").asString();
            partName=prop.check("name",Value("controlboard"), "prefix for port names").asString();
            rootName+=(partName);
            if( rootName.find("//") != std::string::npos )
            {
                rootName.replace(rootName.find("//"), 2, "/");
            }

            ///// We now open ports, then attach the readers or callbacks
            if(! inputRPCPort.open((rootName+"/rpc:i")) )
            {
                yError() <<"Error opening port "<< rootName+"/rpc:i\n";
                success = false;
                break;
            }
            inputRPCPort.setReader(RPC_parser);
            inputRPC_buffer.attach(inputRPCPort);
            RPC_parser.attach(inputRPC_buffer);

            if(!inputStreamingPort.open(rootName+"/command:i") )
            {
                yError() <<"Error opening port "<< rootName+"/rpc:i\n";
                success = false;
                break;
            }

            // attach callback.
            inputStreamingPort.setStrict();
            inputStreamingPort.useCallback(streaming_parser);

            if(!outputPositionStatePort.open(rootName+"/state:o") )
            {
                yError() <<"Error opening port "<< rootName+"/state:o\n";
                success = false;
                break;
            }

            // new extended output state port
            if(!extendedOutputStatePort.open(rootName+"/stateExt:o") )
            {
                yError() <<"Error opening port "<< rootName+"/state:o\n";
                success = false;
                break;
            }
            extendedOutputState_buffer.attach(extendedOutputStatePort);
            success = true;
        } break;
    }  // end switch

    // cleanup if something went wrong
    if(!success)
    {
        cleanup_yarpPorts();
    }
    return success;
}


bool ControlBoardWrapper::open(Searchable& config)
{
    Property prop;
    prop.fromString(config.toString());

    _verb = (prop.check("verbose","if present, give detailed output"));
    if (_verb)
        yInfo("ControlBoardWrapper: running with verbose output\n");

    if(!checkPortName(config) )
    {
        yError() << "the portName was not correctly set, check you r configuration file";
        return false;
    }

    // check FIRST for deprecated parameter
    if(prop.check("threadrate"))
    {
        yError() << " *** ControlBoardWrapper2 is using removed parameter 'threadrate', use 'period' instead ***";
        return false;
    }

    // NOW, check for correct parameter, so if both are present we use the correct one
    if(prop.check("period"))
    {
        if(!prop.find("period").isInt32())
        {
            yError() << " *** ControlBoardWrapper2: 'period' parameter is not an integer value *** ";
            return false;
        }
        period = prop.find("period").asInt32() / 1000.0;
        if(period <= 0)
        {
            yError() << " *** ControlBoardWrapper2: 'period' parameter is not valid, read value is " << period << " ***";
            return false;
        }
    }
    else
    {
        yDebug() << "ControlBoardWrapper2: 'period' parameter missing, using default thread period = 20ms";
        period = 0.02;
    }

    // check if we need to create subdevice or if they are
    // passed later on thorugh attachAll()
    if(prop.check("subdevice"))
    {
        ownDevices=true;
        prop.setMonitor(config.getMonitor());
        if(! openAndAttachSubDevice(prop))
        {
            yError("ControlBoardWrapper: error while opening subdevice\n");
            return false;
        }
    }
    else
    {
        ownDevices=false;
        if(!openDeferredAttach(prop))
            return false;
    }

    // using controlledJoints here will allocate more memory than required, but not so much.
    rpcData.resize(device.subdevices.size(), controlledJoints, &device);

     /* This must be after the openAndAttachSubDevice() or openDeferredAttach() in order to have the correct number of controlledJoints,
        but before the initialize_ROS and initialize_YARP */
    if(!checkROSParams(config) )
    {
        yError() << partName << " ROS parameter are not correct, check your configuration file";
        return false;
    }

    // call ROS node/topic initialization, if needed
    if(!initialize_ROS() )
    {
        return false;
    }

    // call YARP port initialization, if needed
    if(!initialize_YARP(prop) )
    {
        yError() << partName << "Something wrong when initting yarp ports";
        return false;
    }

    times.resize(controlledJoints);
    ros_struct.name.resize(controlledJoints);
    ros_struct.position.resize(controlledJoints);
    ros_struct.velocity.resize(controlledJoints);
    ros_struct.effort.resize(controlledJoints);

    // In case attach is not deferred and the controlboard already owns a valid device
    // we can start the thread. Otherwise this will happen when attachAll is called
    if (ownDevices)
    {
       PeriodicThread::setPeriod(period);
       if (!PeriodicThread::start())
           return false;
    }
    return true;
}


// Default usage
// Open the wrapper only, the attach method needs to be called before using it
bool ControlBoardWrapper::openDeferredAttach(Property& prop)
{
    if (!prop.check("networks", "list of networks merged by this wrapper"))
    {
        yError() << "controlBoardWrapper2: List of networks to attach to was not found.\n";
        return false;
    }

    Bottle *nets=prop.find("networks").asList();
    if(nets==nullptr)
    {
       yError() <<"Error parsing parameters: \"networks\" should be followed by a list\n";
       return false;
    }

    if (!prop.check("joints", "number of joints of the part"))
        return false;

    controlledJoints=prop.find("joints").asInt32();

    int nsubdevices=nets->size();
    device.lut.resize(controlledJoints);
    device.subdevices.resize(nsubdevices);

    // configure the devices
    int totalJ=0;
    for(size_t k=0;k<nets->size();k++)
    {
        Bottle parameters;
        int wBase;
        int wTop;

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
            wBase=bot->get(0).asInt32();
            wTop=bot->get(1).asInt32();
            base=bot->get(2).asInt32();
            top=bot->get(3).asInt32();
        }
        else if (parameters.size()==5)
        {
            // yError<<"Parameter networks use deprecated syntax\n";
            wBase=parameters.get(1).asInt32();
            wTop=parameters.get(2).asInt32();
            base=parameters.get(3).asInt32();
            top=parameters.get(4).asInt32();
        }
        else
        {
            yError() <<"Error: check network parameters in part description"
                     <<"--> I was expecting "<<nets->get(k).asString().c_str() << " followed by a list of four integers in parenthesis"
                     <<"Got: "<< parameters.toString().c_str() << "\n";
            return false;
        }

        SubDevice *tmpDevice = device.getSubdevice(k);
        if (!tmpDevice)
        {
            yError() << "get of subdevice returned null";
            return false;
        }

        tmpDevice->setVerbose(_verb);

        int axes=top-base+1;
        if (!tmpDevice->configure(wBase, wTop, base, top, axes, nets->get(k).asString(), this))
        {
            yError() <<"configure of subdevice ret false";
            return false;
        }

        // Check input values are in range
        if( (wBase < 0) || (wBase >= controlledJoints) )
        {
            yError() << "ControlBoardWrapper input configuration for device " << partName << "has a wrong attach map.\n" << \
                        "First index " << wBase << "must be inside range from 0 to 'joints' ("<< controlledJoints << ")";
            return false;
        }

        if( (wTop < 0) || (wTop >= controlledJoints) )
        {
            yError() << "ControlBoardWrapper input configuration for device " << partName << "has a wrong attach map.\n" << \
                        "Second index " << wTop << "must be inside range from 0 to 'joints' ("<< controlledJoints << ")";
            return false;
        }

        if(wBase > wTop)
        {
            yError() << "ControlBoardWrapper input configuration for device " << partName << "has a wrong attach map.\n" << \
                        "First index " << wBase << "must be lower than  second index " << wTop;
            return false;
        }

        for(int j=wBase, jInDev=base;j<=wTop;j++, jInDev++)
        {
            device.lut[j].deviceEntry=k;
            device.lut[j].offset=j-wBase;
            device.lut[j].jointIndexInDev=jInDev;
        }

        totalJ+=axes;
    }

    if (totalJ!=controlledJoints)
    {
        yError() <<"Error total number of mapped joints ("<< totalJ <<") does not correspond to part joints (" << controlledJoints << ")";
        return false;
    }
    return true;
}

// For the simulator, if a subdevice parameter is given to the wrapper, it will
// open it and attach to immediately.
bool ControlBoardWrapper::openAndAttachSubDevice(Property& prop)
{
    Property p;
    subDeviceOwned = new PolyDriver;
    p.fromString(prop.toString());

    std::string subdevice = prop.find("subdevice").asString();
    p.setMonitor(prop.getMonitor(), subdevice.c_str()); // pass on any monitoring
    p.unput("device");
    p.put("device", subdevice);  // subdevice was already checked before

    // if errors occurred during open, quit here.
    yDebug("opening controlBoardWrapper2 subdevice\n");
    subDeviceOwned->open(p);

    if (!subDeviceOwned->isValid())
    {
        yError("opening controlBoardWrapper2 subdevice... FAILED\n");
        return false;
    }

    yarp::dev::IEncoders * iencs = nullptr;

    subDeviceOwned->view(iencs);

    if (iencs == nullptr)
    {
        yError("Opening IEncoders interface of controlBoardWrapper2 subdevice... FAILED\n");
        return false;
    }

    bool getAx = iencs->getAxes(&controlledJoints);

    if (!getAx)
    {
        yError("Calling getAxes of controlBoardWrapper2 subdevice... FAILED\n");
        return false;
    }
    yDebug("joints parameter is %d\n", controlledJoints);


    device.lut.resize(controlledJoints);
    device.subdevices.resize(1);

    // configure the device
    base = 0;
    top  = controlledJoints-1;

    int wbase = base;
    int wtop = top;
    SubDevice *tmpDevice=device.getSubdevice(0);
    tmpDevice->setVerbose(_verb);

    std::string subDevName ((partName + "_" + subdevice));
    if (!tmpDevice->configure(wbase, wtop, base, top, controlledJoints, subDevName, this) )
    {
        yError() <<"configure of subdevice ret false";
        return false;
    }

    for(int j=0; j<controlledJoints; j++)
    {
        device.lut[j].deviceEntry = 0;
        device.lut[j].offset = j;
    }


    if (!device.subdevices[0].attach(subDeviceOwned, subDevName))
        return false;

    // initialization.
    RPC_parser.initialize();
    updateAxisName();
    calculateMaxNumOfJointsInDevices();
    return true;
}

void ControlBoardWrapper::calculateMaxNumOfJointsInDevices()
{
    device.maxNumOfJointsInDevices = 0;

    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p = device.getSubdevice(d);
        if(p->totalAxis > device.maxNumOfJointsInDevices)
            device.maxNumOfJointsInDevices = p->totalAxis;
    }
}

bool ControlBoardWrapper::updateAxisName()
{
    // If attached device has axisName update the internal values, otherwise keep the on from wrapper
    // config file, if any.
    // IMPORTANT!! This function has to be called BEFORE the thread starts, because if ROS is enabled,
    // the name has to be correct right from the first message!!

    // FOR THE FUTURE: this double version will be dropped because it'll create confusion. Only the names
    // from the motionControl device will be considered good

    // no need to update this variable if we are not using ROS. Yarp RPC will always call the sudevice.
    if(useROS == ROS_disabled )
        return true;

    std::string tmp;
    // I need a temporary vector because if I'm wrapping more than one subdevice, and one of them
    // does not have the axesName, then I'd stick with the old names from wrpper config file, if any.
    vector<string> tmpVect;
    bool ret = true;

    tmpVect.clear();
    for(int i=0; i < controlledJoints; i++)
    {
        if( (ret = getAxisName(i, tmp) && ret) )
        {
            std::string tmp2(tmp);
            tmpVect.push_back(tmp2);
        }
    }

    if(ret)
    {
        if(jointNames.size() != 0)
        {
            yWarning() << "Found 2 instance of jointNames parameter: one in the wrapper [ROS] group and another one in the subdevice, the latter one will be used.";
            std::string fullNames;
            for(int i=0; i < controlledJoints; i++)  fullNames.append(tmpVect[i]);
        }

        jointNames.clear();
        jointNames = tmpVect;
    }
    else
    {
        if(jointNames.size() == 0)
        {
            yError() << "Joint names were not found! they are mandatory when using ROS topic";
            return false;
        }
        else
        {
            yWarning() <<
            "\n************************************************************************************************** \n" <<
            "* Joint names for ROS topic were found in the [ROS] group in the wrapper config file for\n" <<
            "* '" << partName << "' device.\n" <<
            "* They should be in the MotionControl device(s) instead. Please update the config files.\n" <<
            "**************************************************************************************************";
        }
    }
    return true;
}


bool ControlBoardWrapper::attachAll(const PolyDriverList &polylist)
{
    //check if we already instantiated a subdevice previously
    if (ownDevices)
        return false;

    for(int p=0;p<polylist.size();p++)
    {
        // look if we have to attach to a calibrator
        std::string tmpKey=polylist[p]->key;
        if(tmpKey == "Calibrator" || tmpKey == "calibrator")
        {
            // Set the IRemoteCalibrator interface, the wrapper must point to the calibrato rdevice
            yarp::dev::IRemoteCalibrator *calibrator;
            polylist[p]->poly->view(calibrator);

            IRemoteCalibrator::setCalibratorDevice(calibrator);
            continue;
        }

        // find appropriate entry in list of subdevices and attach
        unsigned int k=0;
        for(k=0; k<device.subdevices.size(); k++)
        {
            if (device.subdevices[k].id==tmpKey)
            {
                if (!device.subdevices[k].attach(polylist[p]->poly, tmpKey))
                {
                    yError("ControlBoardWrapper: attach to subdevice %s failed\n", polylist[p]->key.c_str());
                    return false;
                }
            }
        }
    }

    //check if all devices are attached to the driver
    bool ready=true;
    for(auto& subdevice : device.subdevices)
    {
        if (!subdevice.isAttached())
        {
            yError("ControlBoardWrapper: device %s was not found in the list passed to attachAll", subdevice.id.c_str());
            ready=false;
        }
    }

    if (!ready)
    {
        yError("ControlBoardWrapper: AttachAll failed, some subdevice was not found or its attach failed\n");
        stringstream ss;
        for(int p=0;p<polylist.size();p++)
        {
            ss << polylist[p]->key.c_str() << " ";
        }
        yError("ControlBoardWrapper: List of devices keys passed to attachAll: %s\n", ss.str().c_str());
        return false;
    }

    // initialization.
    RPC_parser.initialize();

    updateAxisName();
    calculateMaxNumOfJointsInDevices();
    PeriodicThread::setPeriod(period);
    return PeriodicThread::start();
}

bool ControlBoardWrapper::detachAll()
{
        //check if we already instantiated a subdevice previously
        if (ownDevices)
            return false;

        if (yarp::os::PeriodicThread::isRunning())
            yarp::os::PeriodicThread::stop();

        int devices=device.subdevices.size();

        for(int k=0;k<devices;k++) {
            SubDevice* sub = device.getSubdevice(k);
            if (sub) sub->detach();
        }

        IRemoteCalibrator::releaseCalibratorDevice();
        return true;
}

void ControlBoardWrapper::run()
{
    // check we are not overflowing with input messages
    if(inputStreamingPort.getPendingReads() >= 20)
    {
        yWarning() << "number of streaming intput messages to be read is " << inputStreamingPort.getPendingReads() << " and can overflow";
    }

    // Small optimization: Avoid to call getEncoders twice, one for YARP port
    // and again for ROS topic.
    //
    // Calling getStuff here on ros_struct because it is a class member, hence
    // always available. In the other side, to have the yarp struct to write into
    // it will be rewuired to call port.prepare, that it is something I should
    // not do if the wrapper is in ROS_only configuration.

    bool positionsOk = getEncodersTimed(ros_struct.position.data(), times.data());
    bool speedsOk    = getEncoderSpeeds(ros_struct.velocity.data());
    bool torqueOk    = getTorques(ros_struct.effort.data());

    // Update the port envelope time by averaging all timestamps
    time.update(std::accumulate(times.begin(), times.end(), 0.0) / controlledJoints);

    if(useROS != ROS_only)
    {
        // handle stateExt first
        jointData &yarp_struct = extendedOutputState_buffer.get();

        yarp_struct.jointPosition.resize(controlledJoints);
        yarp_struct.jointVelocity.resize(controlledJoints);
        yarp_struct.jointAcceleration.resize(controlledJoints);
        yarp_struct.motorPosition.resize(controlledJoints);
        yarp_struct.motorVelocity.resize(controlledJoints);
        yarp_struct.motorAcceleration.resize(controlledJoints);
        yarp_struct.torque.resize(controlledJoints);
        yarp_struct.pwmDutycycle.resize(controlledJoints);
        yarp_struct.current.resize(controlledJoints);
        yarp_struct.controlMode.resize(controlledJoints);
        yarp_struct.interactionMode.resize(controlledJoints);

        // Get already stored data from before. This is to avoid a double call to HW device,
        // which may require more time.        //
        yarp_struct.jointPosition_isValid       = positionsOk;
        std::copy(ros_struct.position.begin(), ros_struct.position.end(),  yarp_struct.jointPosition.begin());

        yarp_struct.jointVelocity_isValid       = speedsOk;
        std::copy(ros_struct.velocity.begin(), ros_struct.velocity.end(),  yarp_struct.jointVelocity.begin());

        yarp_struct.torque_isValid              = torqueOk;
        std::copy(ros_struct.effort.begin(), ros_struct.effort.end(),  yarp_struct.torque.begin());

        // Get remaining data from HW
        yarp_struct.jointAcceleration_isValid   = getEncoderAccelerations(yarp_struct.jointAcceleration.data());
        yarp_struct.motorPosition_isValid       = getMotorEncoders(yarp_struct.motorPosition.data());
        yarp_struct.motorVelocity_isValid       = getMotorEncoderSpeeds(yarp_struct.motorVelocity.data());
        yarp_struct.motorAcceleration_isValid   = getMotorEncoderAccelerations(yarp_struct.motorAcceleration.data());
        yarp_struct.torque_isValid              = getTorques(yarp_struct.torque.data());
        yarp_struct.pwmDutycycle_isValid        = getDutyCycles(yarp_struct.pwmDutycycle.data());
        yarp_struct.current_isValid             = getCurrents(yarp_struct.current.data());
        yarp_struct.controlMode_isValid         = getControlModes(yarp_struct.controlMode.data());
        yarp_struct.interactionMode_isValid     = getInteractionModes((yarp::dev::InteractionModeEnum* ) yarp_struct.interactionMode.data());

        extendedOutputStatePort.setEnvelope(time);
        extendedOutputState_buffer.write();

        // handle state:o
        yarp::sig::Vector& v = outputPositionStatePort.prepare();
        v.resize(controlledJoints);
        std::copy(yarp_struct.jointPosition.begin(), yarp_struct.jointPosition.end(), v.begin());

        outputPositionStatePort.setEnvelope(time);
        outputPositionStatePort.write();
    }

    if(useROS != ROS_disabled)
    {
        // Data from HW have been gathered few lines before
        JointTypeEnum jType;
        for(int i=0; i< controlledJoints; i++)
        {
            getJointType(i, jType);
            if(jType == VOCAB_JOINTTYPE_REVOLUTE)
            {
                ros_struct.position[i] = convertDegreesToRadians(ros_struct.position[i]);
                ros_struct.velocity[i] = convertDegreesToRadians(ros_struct.velocity[i]);
            }
        }

        ros_struct.name=jointNames;

        ros_struct.header.seq = rosMsgCounter++;
        ros_struct.header.stamp = time.getTime();

        rosPublisherPort.write(ros_struct);
    }
}

//
//  IPid Interface
//
bool ControlBoardWrapper::setPid(const PidControlTypeEnum& pidtype, int j, const Pid &p)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *s=device.getSubdevice(subIndex);
    if (!s)
        return false;

    if (s->pid)
    {
        return s->pid->setPid(pidtype, off+s->base, p);
    }
    return false;
}

bool ControlBoardWrapper::setPids(const PidControlTypeEnum& pidtype, const Pid *ps)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            ret=ret&&p->pid->setPid(pidtype, off+p->base, ps[l]);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::setPidReference(const PidControlTypeEnum& pidtype, int j, double ref)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->setPidReference(pidtype, off+p->base, ref);
    }
    return false;
}

bool ControlBoardWrapper::setPidReferences(const PidControlTypeEnum& pidtype, const double *refs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            ret=ret&&p->pid->setPidReference(pidtype, off+p->base, refs[l]);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::setPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double limit)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->setPidErrorLimit(pidtype, off+p->base, limit);
    }
    return false;
}

bool ControlBoardWrapper::setPidErrorLimits(const PidControlTypeEnum& pidtype, const double *limits)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            ret=ret&&p->pid->setPidErrorLimit(pidtype, off+p->base, limits[l]);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getPidError(const PidControlTypeEnum& pidtype, int j, double *err)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->getPidError(pidtype, off+p->base, err);
    }
    *err = 0.0;
    return false;
}

bool ControlBoardWrapper::getPidErrors(const PidControlTypeEnum& pidtype, double *errs)
{
    auto* errors = new double [device.maxNumOfJointsInDevices];

    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }
        if( (p->pid) &&(ret = p->pid->getPidErrors(pidtype, errors)) )
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                errs[juser] = errors[jdevice];
            }
        }
        else
        {
            printError("getPidErrors", p->id, ret);
            ret =  false;
            break;
        }
    }

    delete[] errors;
    return ret;
}

bool ControlBoardWrapper::getPidOutput(const PidControlTypeEnum& pidtype, int j, double *out)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->getPidOutput(pidtype, off+p->base, out);
    }
    *out=0.0;
    return false;
}

bool ControlBoardWrapper::getPidOutputs(const PidControlTypeEnum& pidtype, double *outs)
{
    auto* outputs = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->pid) &&(ret = p->pid->getPidOutputs(pidtype, outputs)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                outs[juser] = outputs[jdevice];
            }
        }
        else
        {
            printError("getPidOutouts", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] outputs;
    return ret;
}

bool ControlBoardWrapper::setPidOffset(const PidControlTypeEnum& pidtype, int j, double v)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->setPidOffset(pidtype, off+p->base, v);
    }
    return false;
}

bool ControlBoardWrapper::getPid(const PidControlTypeEnum& pidtype, int j, Pid *p)
{
//#warning "check for max number of joints!?!?!"
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *s=device.getSubdevice(subIndex);
    if (!s)
        return false;

    if (s->pid)
    {
        return s->pid->getPid(pidtype, off+s->base, p);
    }
    return false;
}

bool ControlBoardWrapper::getPids(const PidControlTypeEnum& pidtype, Pid *pids)
{
    Pid *pids_device = new Pid[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->pid) &&(ret = p->pid->getPids(pidtype, pids_device)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                pids[juser] = pids_device[jdevice];
            }
        }
        else
        {
            printError("getPids", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] pids_device;
    return ret;
}

bool ControlBoardWrapper::getPidReference(const PidControlTypeEnum& pidtype, int j, double *ref) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;
    if (p->pid)
    {
        return p->pid->getPidReference(pidtype, off+p->base, ref);
    }
    return false;
}

bool ControlBoardWrapper::getPidReferences(const PidControlTypeEnum& pidtype, double *refs)
{
    auto* references = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->pid) &&(ret = p->pid->getPidReferences(pidtype, references)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                refs[juser] = references[jdevice];
            }
        }
        else
        {
            printError("getPidReferences", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] references;
    return ret;
}

bool ControlBoardWrapper::getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double *limit) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->getPidErrorLimit(pidtype, off+p->base, limit);
    }
    return false;
}

bool ControlBoardWrapper::getPidErrorLimits(const PidControlTypeEnum& pidtype, double *limits)
{
    auto* lims = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->pid) &&(ret = p->pid->getPidErrorLimits(pidtype, lims)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                limits[juser] = lims[jdevice];
            }
        }
        else
        {
            printError("getPidErrorLimits", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] lims;
    return ret;
}

bool ControlBoardWrapper::resetPid(const PidControlTypeEnum& pidtype, int j) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->resetPid(pidtype, off+p->base);
    }
    return false;
}

bool ControlBoardWrapper::disablePid(const PidControlTypeEnum& pidtype, int j) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->disablePid(pidtype, off+p->base);
    }
    return false;
}

bool ControlBoardWrapper::enablePid(const PidControlTypeEnum& pidtype, int j) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->enablePid(pidtype, off+p->base);
    }
    return false;
}

bool ControlBoardWrapper::isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool* enabled)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if(p->pid)
        return p->pid->isPidEnabled(pidtype, off+p->base, enabled);

    return false;
}

//
// IPOSITIONCONTROL
//
/* IPositionControl */

/**
* Get the number of controlled axes. This command asks the number of controlled
* axes for the current physical interface.
* @param ax pointer to storage
* @return true/false.
*/
bool ControlBoardWrapper::getAxes(int *ax) {
    *ax=controlledJoints;
    return true;
}

/**
* Set new reference point for a single axis.
* @param j joint number
* @param ref specifies the new ref point
* @return true/false on success/failure
*/
bool ControlBoardWrapper::positionMove(int j, double ref) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pos)
    {
        return p->pos->positionMove(off+p->base, ref);
    }

    return false;
}

/** Set new reference point for all axes.
* @param refs array, new reference points.
* @return true/false on success/failure
*/
bool ControlBoardWrapper::positionMove(const double *refs)
{
    bool ret = true;
    int j_wrap = 0;         // index of the wrapper joint

    int nDev = device.subdevices.size();
    for(int subDev_idx=0; subDev_idx < nDev; subDev_idx++)
    {
        int subIndex=device.lut[j_wrap].deviceEntry;
        SubDevice *p=device.getSubdevice(subIndex);

        if (!p)
        {
            return false;
        }

        int wrapped_joints=(p->top - p->base) + 1;
        int *joints = new int[wrapped_joints];

        if(p->pos)
        {
            // versione comandi su subset di giunti
            for(int j_dev = 0; j_dev < wrapped_joints; j_dev++)
            {
                joints[j_dev] = p->base + j_dev;  // for all joints is equivalent to add offset term
            }

            ret = ret && p->pos->positionMove(wrapped_joints, joints, &refs[j_wrap]);
            j_wrap+=wrapped_joints;
        }
        else
        {
            ret=false;
        }

        if(joints!=nullptr)
        { delete [] joints;
          joints = nullptr;}
    }

    return ret;
}

/** Set new reference point for a subset of axis.
 * @param joints pointer to the array of joint numbers
 * @param refs   pointer to the array specifying the new reference points
 * @return true/false on success/failure
 */
bool ControlBoardWrapper::positionMove(const int n_joints, const int *joints, const double *refs)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for(int j=0; j<n_joints; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base;
        rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = refs[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->pos)
        {
            ret= ret && rpcData.subdevices_p[subIndex]->pos->positionMove(rpcData.subdev_jointsVectorLen[subIndex],
                                                                           rpcData.jointNumbers[subIndex],
                                                                           rpcData.values[subIndex]);
        }
        else
        {
            ret=false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

bool ControlBoardWrapper::getTargetPosition(const int j, double* ref)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);

    if (!p)
        return false;

    if (p->pos)
    {
        bool ret = p->pos->getTargetPosition(off+p->base, ref);
        return ret;
    }
    *ref=0;
    return false;
}


/** Get reference speed of all joints. These are the  values used during the
* interpolation of the trajectory.
* @param spds pointer to the array that will store the speed values.
* @return true/false on success/failure.
*/
bool ControlBoardWrapper::getTargetPositions(double *spds)
{
    auto* targets = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->pos) &&(ret = p->pos->getTargetPositions(targets)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                spds[juser] = targets[jdevice];
            }
        }
        else
        {
            printError("getTargetPositions", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] targets;
    return ret;
}


bool ControlBoardWrapper::getTargetPositions(const int n_joints, const int *joints, double *targets)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for(int j=0; j<n_joints; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base;
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->pos)
        {
            ret= ret && rpcData.subdevices_p[subIndex]->pos->getTargetPositions( rpcData.subdev_jointsVectorLen[subIndex],
                                                                            rpcData.jointNumbers[subIndex],
                                                                            rpcData.values[subIndex]);
        }
    }

    if(ret)
    {
        // ReMix values by user expectations
        for(int i=0; i<rpcData.deviceNum; i++)
            rpcData.subdev_jointsVectorLen[i]=0;                  // reset tmp index

        // fill the output vector
        for(int j=0; j<n_joints; j++)
        {
            subIndex = device.lut[joints[j]].deviceEntry;
            targets[j]  = rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]];
            rpcData.subdev_jointsVectorLen[subIndex]++;
        }
    }
    else
    {
        for(int j=0; j<n_joints; j++)
        {
            targets[j] = 0;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

/** Set relative position. The command is relative to the
* current position of the axis.
* @param j joint axis number
* @param delta relative command
* @return true/false on success/failure
*/
bool ControlBoardWrapper::relativeMove(int j, double delta) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pos)
    {
        return p->pos->relativeMove(off+p->base, delta);
    }

    return false;
}

/** Set relative position, all joints.
* @param deltas pointer to the relative commands
* @return true/false on success/failure
*/
bool ControlBoardWrapper::relativeMove(const double *deltas) {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pos)
        {
            ret=ret&&p->pos->relativeMove(off+p->base, deltas[l]);
        }
        else
            ret=false;
    }
    return ret;
}

/** Set relative position for a subset of joints.
 * @param joints pointer to the array of joint numbers
 * @param deltas pointer to the array of relative commands
 * @return true/false on success/failure
 */
bool ControlBoardWrapper::relativeMove(const int n_joints, const int *joints, const double *deltas)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for(int j=0; j<n_joints; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base;
        rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = deltas[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->pos)
        {
            ret= ret && rpcData.subdevices_p[subIndex]->pos->relativeMove(rpcData.subdev_jointsVectorLen[subIndex],
                                                                           rpcData.jointNumbers[subIndex],
                                                                           rpcData.values[subIndex]);
        }
        else
        {
            ret=false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

/**
* Check if the current trajectory is terminated. Non blocking.
* @param j the axis
* @param flag true if the trajectory is terminated, false otherwise
* @return false on failure
*/
bool ControlBoardWrapper::checkMotionDone(int j, bool *flag) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);

    if (!p)
        return false;

    if (p->pos)
    {
        return p->pos->checkMotionDone(off+p->base, flag);
    }

    return false;
}

/**
* Check if the current trajectory is terminated. Non blocking.
* @param flag true if the trajectory is terminated, false otherwise
* @return false on failure
*/
bool ControlBoardWrapper::checkMotionDone(bool *flag)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    // In this case the "all joint version" of checkMotionDone(bool *flag) cannot be
    // called because the return value is an 'and' of all joints.
    // Therefore only the corret joints must be evaluated.

    int subIndex = 0;
    for(int j=0; j<controlledJoints; j++)
    {
        subIndex = device.lut[j].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = device.lut[j].offset + rpcData.subdevices_p[subIndex]->base;
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    bool tmp_subdeviceDone  = true;
    bool tmp_deviceDone     = true;

    // for each subdevice wrapped call checkmotiondone only on interested joints
    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->pos)
        {
            ret= ret && rpcData.subdevices_p[subIndex]->pos->checkMotionDone( rpcData.subdev_jointsVectorLen[subIndex],
                                                                              rpcData.jointNumbers[subIndex],
                                                                              &tmp_subdeviceDone);
            tmp_deviceDone &= tmp_subdeviceDone;
        }
    }
    rpcDataMutex.unlock();

    // return a single value to the caller
    *flag = tmp_deviceDone;
    return ret;
}


/** Check if the current trajectory is terminated. Non blocking.
 * @param joints pointer to the array of joint numbers
 * @param flag   true if the trajectory is terminated, false otherwise
 *               (a single value which is the 'and' of all joints')
 * @return true/false if network communication went well.
 */
bool ControlBoardWrapper::checkMotionDone(const int n_joints, const int *joints, bool *flags)
{
    bool ret = true;
    bool tmp = true;
    bool XFlags = true;

   rpcDataMutex.lock();
   //Reset subdev_jointsVectorLen vector
   memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

   // Create a map of joints for each subDevice
   int subIndex = 0;
   for(int j=0; j<n_joints; j++)
   {
       subIndex = device.lut[joints[j]].deviceEntry;
       rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base;
       rpcData.subdev_jointsVectorLen[subIndex]++;
   }

   for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
   {
       if(rpcData.subdevices_p[subIndex]->pos)
       {
           ret= ret && rpcData.subdevices_p[subIndex]->pos->checkMotionDone(rpcData.subdev_jointsVectorLen[subIndex],
                                                                             rpcData.jointNumbers[subIndex],
                                                                             &XFlags);
           tmp = tmp && XFlags;
       }
       else
       {
           ret=false;
       }
   }
    if(ret)
        *flags = tmp;
    else
        *flags = false;
    rpcDataMutex.unlock();
    return ret;
}

/** Set reference speed for a joint, this is the speed used during the
* interpolation of the trajectory.
* @param j joint number
* @param sp speed value
* @return true/false upon success/failure
*/
bool ControlBoardWrapper::setRefSpeed(int j, double sp) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pos)
    {
        return p->pos->setRefSpeed(off+p->base, sp);
    }
    return false;
}

/** Set reference speed on all joints. These values are used during the
* interpolation of the trajectory.
* @param spds pointer to the array of speed values.
* @return true/false upon success/failure
*/
bool ControlBoardWrapper::setRefSpeeds(const double *spds)
{
    bool ret = true;
    int j_wrap = 0;         // index of the wrapper joint

    for(unsigned int subDev_idx=0; subDev_idx < device.subdevices.size(); subDev_idx++)
    {
        SubDevice *p=device.getSubdevice(subDev_idx);

        if(!p)
            return false;

        int wrapped_joints=(p->top - p->base) + 1;
        int *joints = new int[wrapped_joints];

        if(p->pos)
        {
            // verione comandi su subset di giunti
            for(int j_dev = 0; j_dev < wrapped_joints; j_dev++)
            {
                joints[j_dev] = p->base + j_dev;
            }

            ret = ret && p->pos->setRefSpeeds(wrapped_joints, joints, &spds[j_wrap]);
            j_wrap += wrapped_joints;
        }
        else
        {
            ret=false;
        }

        if(joints!=nullptr)
        { delete [] joints;
          joints = nullptr;}
    }

    return ret;
}


/** Set reference speed on all joints. These values are used during the
 * interpolation of the trajectory.
 * @param joints pointer to the array of joint numbers
 * @param spds   pointer to the array with speed values.
 * @return true/false upon success/failure
 */
bool ControlBoardWrapper::setRefSpeeds(const int n_joints, const int *joints, const double *spds)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for(int j=0; j<n_joints; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] =  device.lut[joints[j]].offset +
                                                                            rpcData.subdevices_p[subIndex]->base;
        rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = spds[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->pos)
        {
            ret= ret && rpcData.subdevices_p[subIndex]->pos->setRefSpeeds( rpcData.subdev_jointsVectorLen[subIndex],
                                                                            rpcData.jointNumbers[subIndex],
                                                                            rpcData.values[subIndex]);
        }
        else
        {
            ret=false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

/** Set reference acceleration for a joint. This value is used during the
* trajectory generation.
* @param j joint number
* @param acc acceleration value
* @return true/false upon success/failure
*/
bool ControlBoardWrapper::setRefAcceleration(int j, double acc) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);

    if (!p)
        return false;

    if (p->pos)
    {
        return p->pos->setRefAcceleration(off+p->base, acc);
    }
    return false;
}

/** Set reference acceleration on all joints. This is the valure that is
* used during the generation of the trajectory.
* @param accs pointer to the array of acceleration values
* @return true/false upon success/failure
*/
bool ControlBoardWrapper::setRefAccelerations(const double *accs)
{
    bool ret = true;
    int j_wrap = 0;    // index of the joint from the wrapper side (useful if wrapper joins 2 subdevices)

    // for all subdevices
    for(unsigned int subDev_idx=0; subDev_idx < device.subdevices.size(); subDev_idx++)
    {
        SubDevice *p=device.getSubdevice(subDev_idx);

        if(!p)
            return false;

        int wrapped_joints=(p->top - p->base) + 1;
        int *joints = new int[wrapped_joints];  // to be defined once and for all?

        if(p->pos)
        {
            // verione comandi su subset di giunti
            for(int j_dev = 0; j_dev < wrapped_joints; j_dev++)
            {
                joints[j_dev] = p->base + j_dev;
            }

            ret = ret && p->pos->setRefAccelerations(wrapped_joints, joints, &accs[j_wrap]);
            j_wrap += wrapped_joints;
        }
        else
        {
            ret=false;
        }

        if(joints!=nullptr)
        { delete [] joints;
        joints = nullptr;}
    }

    return ret;
}

/** Set reference acceleration on all joints. This is the valure that is
 * used during the generation of the trajectory.
 * @param joints pointer to the array of joint numbers
 * @param accs   pointer to the array with acceleration values
 * @return true/false upon success/failure
 */
bool ControlBoardWrapper::setRefAccelerations(const int n_joints, const int *joints, const double *accs)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for(int j=0; j<n_joints; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] =  device.lut[joints[j]].offset +
        rpcData.subdevices_p[subIndex]->base;
        rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = accs[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->pos)
        {
            ret= ret && rpcData.subdevices_p[subIndex]->pos->setRefAccelerations(  rpcData.subdev_jointsVectorLen[subIndex],
                                                                                    rpcData.jointNumbers[subIndex],
                                                                                    rpcData.values[subIndex]);
        }
        else
        {
            ret=false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}


/** Get reference speed for a joint. Returns the speed used to
 * generate the trajectory profile.
 * @param j joint number
 * @param ref pointer to storage for the return value
 * @return true/false on success or failure
 */
bool ControlBoardWrapper::getRefSpeed(int j, double *ref) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);

    if (!p)
        return false;

    if (p->pos)
    {
        return p->pos->getRefSpeed(off+p->base, ref);
    }
    *ref=0;
    return false;
}


/** Get reference speed of all joints. These are the  values used during the
* interpolation of the trajectory.
* @param spds pointer to the array that will store the speed values.
* @return true/false on success/failure.
*/
bool ControlBoardWrapper::getRefSpeeds(double *spds)
{
    auto* references = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->pos) &&(ret = p->pos->getRefSpeeds(references)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                spds[juser] = references[jdevice];
            }
        }
        else
        {
            printError("getRefSpeeds", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] references;
    return ret;
}


/** Get reference speed of all joints. These are the  values used during the
 * interpolation of the trajectory.
 * @param joints pointer to the array of joint numbers
 * @param spds   pointer to the array that will store the speed values.
 * @return true/false upon success/failure
 */
bool ControlBoardWrapper::getRefSpeeds(const int n_joints, const int *joints, double *spds)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for(int j=0; j<n_joints; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base;
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->pos)
        {
            ret= ret && rpcData.subdevices_p[subIndex]->pos->getRefSpeeds( rpcData.subdev_jointsVectorLen[subIndex],
                                                                            rpcData.jointNumbers[subIndex],
                                                                            rpcData.values[subIndex]);
        }
        else
        {
            ret=false;
        }
    }

    if(ret)
    {
        // ReMix values by user expectations
        for(int i=0; i<rpcData.deviceNum; i++)
            rpcData.subdev_jointsVectorLen[i]=0;                  // reset tmp index

        // fill the output vector
        for(int j=0; j<n_joints; j++)
        {
            subIndex = device.lut[joints[j]].deviceEntry;
            spds[j]  = rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]];
            rpcData.subdev_jointsVectorLen[subIndex]++;
        }
    }
    else
    {
        for(int j=0; j<n_joints; j++)
        {
            spds[j] = 0;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

/** Get reference acceleration for a joint. Returns the acceleration used to
* generate the trajectory profile.
* @param j joint number
* @param acc pointer to storage for the return value
* @return true/false on success/failure
*/
bool ControlBoardWrapper::getRefAcceleration(int j, double *acc) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);

    if (!p)
        return false;

    if (p->pos)
    {
        return p->pos->getRefAcceleration(off+p->base, acc);
    }
    *acc=0;
    return false;
}


/** Get reference acceleration of all joints. These are the values used during the
* interpolation of the trajectory.
* @param accs pointer to the array that will store the acceleration values.
* @return true/false on success or failure
*/
bool ControlBoardWrapper::getRefAccelerations(double *accs)
{
    auto* references = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->pos) &&(ret = p->pos->getRefAccelerations(references)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                accs[juser] = references[jdevice];
            }
        }
        else
        {
            printError("getRefAccelerations", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] references;
    return ret;
}


/** Get reference acceleration for a joint. Returns the acceleration used to
 * generate the trajectory profile.
 * @param joints pointer to the array of joint numbers
 * @param accs   pointer to the array that will store the acceleration values
 * @return true/false on success/failure
 */
bool ControlBoardWrapper::getRefAccelerations(const int n_joints, const int *joints, double *accs)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for(int j=0; j<n_joints; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base;
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->pos)
        {
            ret= ret && rpcData.subdevices_p[subIndex]->pos->getRefAccelerations(  rpcData.subdev_jointsVectorLen[subIndex],
                                                                                    rpcData.jointNumbers[subIndex],
                                                                                    rpcData.values[subIndex]);
        }
        else
        {
            ret=false;
        }
    }

    if(ret)
    {
        // ReMix values by user expectations
        for(int i=0; i<rpcData.deviceNum; i++)
            rpcData.subdev_jointsVectorLen[i]=0;                  // reset tmp index

        // fill the output vector
        for(int j=0; j<n_joints; j++)
        {
            subIndex = device.lut[joints[j]].deviceEntry;
            accs[j]  = rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]];
            rpcData.subdev_jointsVectorLen[subIndex]++;
        }
    }
    else
    {
        for(int j=0; j<n_joints; j++)
        {
            accs[j] = 0;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

/** Stop motion, single joint
* @param j joint number
* @return true/false on success/failure
*/
bool ControlBoardWrapper::stop(int j) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);

    if (!p)
        return false;

    if (p->pos)
    {
        return p->pos->stop(off+p->base);
    }
    return false;
}


/**
* Stop motion, multiple joints
* @return true/false on success/failure
*/
bool ControlBoardWrapper::stop() {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);

        if (!p)
            return false;

        if (p->pos)
        {
            ret=ret&&p->pos->stop(off+p->base);
        }
        else
            ret=false;
    }
    return ret;
}


/** Stop motion for subset of joints
 * @param joints pointer to the array of joint numbers
 * @return true/false on success/failure
 */
bool ControlBoardWrapper::stop(const int n_joints, const int *joints)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for(int j=0; j<n_joints; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] =  device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base;
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->pos)
        {
            ret= ret && rpcData.subdevices_p[subIndex]->pos->stop(rpcData.subdev_jointsVectorLen[subIndex],
                                                                   rpcData.jointNumbers[subIndex]);
        }
        else
        {
            ret=false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}


/* IVelocityControl */

bool ControlBoardWrapper::velocityMove(int j, double v) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);

    if (!p)
        return false;

    if (p->vel)
    {
        return p->vel->velocityMove(off+p->base, v);
    }
    return false;
}

bool ControlBoardWrapper::velocityMove(const double *v)
{
    bool ret = true;
    int j_wrap = 0;         // index of the wrapper joint

    for(unsigned int subDev_idx=0; subDev_idx < device.subdevices.size(); subDev_idx++)
    {
        SubDevice *p=device.getSubdevice(subDev_idx);

        if(!p)
            return false;

        int wrapped_joints=(p->top - p->base) + 1;
        int *joints = new int[wrapped_joints];

        if(p->vel)
        {
            // verione comandi su subset di giunti
            for(int j_dev = 0; j_dev < wrapped_joints; j_dev++)
            {
                joints[j_dev] = p->base + j_dev;
            }

            ret = ret && p->vel->velocityMove(wrapped_joints, joints, &v[j_wrap]);
            j_wrap += wrapped_joints;
        }
        else
        {
            ret=false;
        }

        if(joints!=nullptr)
        { delete [] joints;
          joints = nullptr;}
    }

    return ret;
}

/* IEncoders */

bool ControlBoardWrapper::resetEncoder(int j) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iJntEnc)
    {
        return p->iJntEnc->resetEncoder(off+p->base);
    }
    return false;
}

bool ControlBoardWrapper::resetEncoders() {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iJntEnc)
        {
            ret=ret&&p->iJntEnc->resetEncoder(off+p->base);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::setEncoder(int j, double val) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iJntEnc)
    {
        return p->iJntEnc->setEncoder(off+p->base,val);
    }
    return false;
}

bool ControlBoardWrapper::setEncoders(const double *vals) {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iJntEnc)
        {
            ret=ret&&p->iJntEnc->setEncoder(off+p->base, vals[l]);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getEncoder(int j, double *v) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iJntEnc)
    {
        return p->iJntEnc->getEncoder(off+p->base, v);
    }
    *v=0.0;
    return false;
}

bool ControlBoardWrapper::getEncoders(double *encs)
{
    auto* encValues = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iJntEnc) &&(ret = p->iJntEnc->getEncoders(encValues)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                encs[juser] = encValues[jdevice];
            }
        }
        else
        {
            printError("getEncoders", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] encValues;
    return ret;

}

bool ControlBoardWrapper::getEncodersTimed(double *encs, double *t)
{
    auto* encValues = new double[device.maxNumOfJointsInDevices];
    auto* tValues = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iJntEnc) &&(ret = p->iJntEnc->getEncodersTimed(encValues, tValues)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                encs[juser] = encValues[jdevice];
                t[juser] = tValues[jdevice];
            }
        }
        else
        {
            printError("getEncodersTimed", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] encValues;
    delete [] tValues;
    return ret;
}

bool ControlBoardWrapper::getEncoderTimed(int j, double *v, double *t) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iJntEnc)
    {
        return p->iJntEnc->getEncoderTimed(off+p->base, v, t);
    }
    *v=0.0;
    return false;
}

bool ControlBoardWrapper::getEncoderSpeed(int j, double *sp) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iJntEnc)
    {
        return p->iJntEnc->getEncoderSpeed(off+p->base, sp);
    }
    *sp=0.0;
    return false;
}

bool ControlBoardWrapper::getEncoderSpeeds(double *spds)
{
    auto* sValues = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iJntEnc) &&(ret = p->iJntEnc->getEncoderSpeeds(sValues)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                spds[juser] = sValues[jdevice];
            }
        }
        else
        {
            printError("getEncoderSpeeds", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] sValues;
    return ret;
}

bool ControlBoardWrapper::getEncoderAcceleration(int j, double *acc) {
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iJntEnc)
    {
        return p->iJntEnc->getEncoderAcceleration(off+p->base,acc);
    }
    *acc=0.0;
    return false;
}

bool ControlBoardWrapper::getEncoderAccelerations(double *accs)
{
    auto* aValues = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iJntEnc) &&(ret = p->iJntEnc->getEncoderAccelerations(aValues)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                accs[juser] = aValues[jdevice];
            }
        }
        else
        {
            printError("getEncoderAccelerations", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] aValues;
    return ret;
}

/* IMotor */
bool ControlBoardWrapper::getNumberOfMotors   (int *num) {
    *num=controlledJoints;
    return true;
}

bool ControlBoardWrapper::getTemperature      (int m, double* val) {
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->imotor)
    {
        return p->imotor->getTemperature(off+p->base, val);
    }
    *val=0.0;
    return false;
}

bool ControlBoardWrapper::getTemperatures     (double *vals)
{
    auto* temps = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->imotor) &&(ret = p->imotor->getTemperatures(temps)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                vals[juser] = temps[jdevice];
            }
        }
        else
        {
            printError("getTemperatures", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] temps;
    return ret;
}

bool ControlBoardWrapper::getTemperatureLimit (int m, double* val) {
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->imotor)
    {
        return p->imotor->getTemperatureLimit(off+p->base, val);
    }
    *val=0.0;
    return false;
}

bool ControlBoardWrapper::setTemperatureLimit (int m, const double val) {
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->imotor)
    {
        return p->imotor->setTemperatureLimit(off+p->base,val);
    }
    return false;
}

bool ControlBoardWrapper::getGearboxRatio(int m, double* val) {
    int off = device.lut[m].offset;
    int subIndex = device.lut[m].deviceEntry;

    SubDevice *p = device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->imotor)
    {
        return p->imotor->getGearboxRatio(off + p->base, val);
    }
    *val = 0.0;
    return false;
}

bool ControlBoardWrapper::setGearboxRatio(int m, const double val) {
    int off = device.lut[m].offset;
    int subIndex = device.lut[m].deviceEntry;

    SubDevice *p = device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->imotor)
    {
        return p->imotor->setGearboxRatio(off + p->base, val);
    }
    return false;
}

/* IRemoteVariables */
bool ControlBoardWrapper::getRemoteVariable(std::string key, yarp::os::Bottle& val) {
    bool b = true;

    for (unsigned int i = 0; i < device.subdevices.size(); i++)
    {
        SubDevice *p = device.getSubdevice(i);

        if (!p) return false;
        if (!p->iVar) return false;
        yarp::os::Bottle tmpval;
        b &= p->iVar->getRemoteVariable(key, tmpval);
        if (b) val.append(tmpval);
    }

    return b;
}

bool ControlBoardWrapper::setRemoteVariable(std::string key, const yarp::os::Bottle& val)
{
    size_t bottle_size = val.size();
    size_t device_size = device.subdevices.size();
    if (bottle_size != device_size)
    {
        yError("ControlBoardWrapper::setRemoteVariable bottle_size != device_size failure");
        return false;
    }

    bool b = true;
    for (unsigned int i = 0; i < device_size; i++)
    {
        SubDevice *p = device.getSubdevice(i);
        if (!p)  { yError("ControlBoardWrapper::setRemoteVariable !p failure"); return false; }
        if (!p->iVar) { yError("ControlBoardWrapper::setRemoteVariable !p->iVar failure"); return false; }
        Bottle* partial_val = val.get(i).asList();
        if (partial_val)
        {
            b &= p->iVar->setRemoteVariable(key, *partial_val);
        }
        else
        {
            yError("ControlBoardWrapper::setRemoteVariable general failure");
            return false;
        }
    }

    return b;
}

bool ControlBoardWrapper::getRemoteVariablesList(yarp::os::Bottle* listOfKeys)
{
    //int off = device.lut[0].offset;
    int subIndex = device.lut[0].deviceEntry;
    SubDevice *p = device.getSubdevice(subIndex);

    if (!p)
        return false;

    if (p->iVar)
    {
        return p->iVar->getRemoteVariablesList(listOfKeys);
    }
    return false;
}

/* IMotorEncoders */

bool ControlBoardWrapper::resetMotorEncoder(int m) {
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMotEnc)
    {
        return p->iMotEnc->resetMotorEncoder(off+p->base);
    }
    return false;
}

bool ControlBoardWrapper::resetMotorEncoders() {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iMotEnc)
        {
            ret=ret&&p->iMotEnc->resetMotorEncoder(off+p->base);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::setMotorEncoder(int m, const double val) {
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMotEnc)
    {
        return p->iMotEnc->setMotorEncoder(off+p->base,val);
    }
    return false;
}

bool ControlBoardWrapper::setMotorEncoders(const double *vals) {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iMotEnc)
        {
            ret=ret&&p->iMotEnc->setMotorEncoder(off+p->base, vals[l]);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::setMotorEncoderCountsPerRevolution(int m, double cpr) {
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMotEnc)
    {
        return p->iMotEnc->setMotorEncoderCountsPerRevolution(off+p->base,cpr);
    }
    return false;
}

bool ControlBoardWrapper::getMotorEncoderCountsPerRevolution(int m, double *cpr) {
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoderCountsPerRevolution(off+p->base, cpr);
    }
    *cpr=0.0;
    return false;
}

bool ControlBoardWrapper::getMotorEncoder(int m, double *v) {
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoder(off+p->base, v);
    }
    *v=0.0;
    return false;
}

bool ControlBoardWrapper::getMotorEncoders(double *encs)
{

    auto* encValues = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iMotEnc) &&(ret = p->iMotEnc->getMotorEncoders(encValues)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                encs[juser] = encValues[jdevice];
            }
        }
        else
        {
            printError("getMotorEncoders", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] encValues;
    return ret;
}

bool ControlBoardWrapper::getMotorEncodersTimed(double *encs, double *t)
{
    auto* encValues = new double[device.maxNumOfJointsInDevices];
    auto* tValues = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iMotEnc) &&(ret = p->iMotEnc->getMotorEncodersTimed(encValues, tValues)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                encs[juser] = encValues[jdevice];
                t[juser] = tValues[jdevice];
            }
        }
        else
        {
            printError("getMotorEncodersTimed", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] encValues;
    delete [] tValues;
    return ret;
}

bool ControlBoardWrapper::getMotorEncoderTimed(int m, double *v, double *t) {
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoderTimed(off+p->base, v, t);
    }
    *v=0.0;
    return false;
}

bool ControlBoardWrapper::getMotorEncoderSpeed(int m, double *sp) {
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoderSpeed(off+p->base, sp);
    }
    *sp=0.0;
    return false;
}

bool ControlBoardWrapper::getMotorEncoderSpeeds(double *spds)
{
    auto* sValues = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iMotEnc) &&(ret = p->iMotEnc->getMotorEncoderSpeeds(sValues)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                spds[juser] = sValues[jdevice];
            }
        }
        else
        {
            printError("getMotorEncoderSpeeds", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] sValues;
    return ret;
}

bool ControlBoardWrapper::getMotorEncoderAcceleration(int m, double *acc) {
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoderAcceleration(off+p->base,acc);
    }
    *acc=0.0;
    return false;
}

bool ControlBoardWrapper::getMotorEncoderAccelerations(double *accs)
{
    auto* aValues = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iMotEnc) &&(ret = p->iMotEnc->getMotorEncoderAccelerations(aValues)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                accs[juser] = aValues[jdevice];
            }
        }
        else
        {
            printError("getMotorEncoderAccelerations", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] aValues;
    return ret;

}


bool ControlBoardWrapper::getNumberOfMotorEncoders(int *num) {
    *num=controlledJoints;
    return true;
}

/* IAmplifierControl */

bool ControlBoardWrapper::enableAmp(int j)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->amp)
    {
        return p->amp->enableAmp(off+p->base);
    }
    return false;
}

bool ControlBoardWrapper::disableAmp(int j)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    bool ret = true;
    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    // Use the newer interface if available, otherwise fallback on the old one.
    if(p->iMode)
    {
        ret = p->iMode->setControlMode(off+p->base, VOCAB_CM_IDLE);
    }
    else
    {
        if (p->pos)
            ret = p->amp->disableAmp(off+p->base);
        else
            ret = false;
    }
    return ret;
}

bool ControlBoardWrapper::getAmpStatus(int *st)
{
    int *status = new int[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->amp) &&(ret = p->amp->getAmpStatus(status)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                st[juser] = status[jdevice];
            }
        }
        else
        {
            printError("getAmpStatus", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] status;
    return ret;
}

bool ControlBoardWrapper::getAmpStatus(int j, int *v)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (p && p->amp)
        {
            return p->amp->getAmpStatus(off+p->base,v);
        }
    *v=0;
    return false;
}

bool ControlBoardWrapper::setMaxCurrent(int j, double v)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->amp)
    {
        return p->amp->setMaxCurrent(off+p->base,v);
    }
    return false;
}

bool ControlBoardWrapper::getMaxCurrent(int j, double* v)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
    {
        *v=0.0;
        return false;
    }

    if (p->amp)
    {
        return p->amp->getMaxCurrent(off+p->base,v);
    }
    *v=0.0;
    return false;
}

bool ControlBoardWrapper::getNominalCurrent(int m, double *val)
{
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if(!p)
    {
        *val=0.0;
        return false;
    }

    if(!p->amp)
    {
        *val=0.0;
        return false;
    }
    return p->amp->getNominalCurrent(off+p->base, val);
}

bool ControlBoardWrapper::getPeakCurrent(int m, double *val)
{
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if(!p)
    {
        *val=0.0;
        return false;
    }

    if(!p->amp)
    {
        *val=0.0;
        return false;
    }
    return p->amp->getPeakCurrent(off+p->base, val);
}

bool ControlBoardWrapper::setPeakCurrent(int m, const double val)
{
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (!p->amp)
    {
        return false;
    }
    return p->amp->setPeakCurrent(off+p->base, val);
}

bool ControlBoardWrapper::setNominalCurrent(int m, const double val)
{
    int off = device.lut[m].offset;
    int subIndex = device.lut[m].deviceEntry;

    SubDevice *p = device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (!p->amp)
    {
        return false;
    }
    return p->amp->setNominalCurrent(off + p->base, val);
}

bool ControlBoardWrapper::getPWM(int m, double* val)
{
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;
    SubDevice *p=device.getSubdevice(subIndex);

    //yDebug() << "CBW2::getPWMlimit j" << off+p->base << " p " << (p?"1":"0") << " amp " << (p->amp?"1":"0");
    if(!p)
    {
        *val=0.0;
        return false;
    }

    if(!p->amp)
    {
        *val=0.0;
        return false;
    }
    return p->amp->getPWM(off+p->base, val);
}
bool ControlBoardWrapper::getPWMLimit(int m, double* val)
{
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    //yDebug() << "CBW2::getPWMlimit j" << off+p->base << " p " << (p?"1":"0") << " amp " << (p->amp?"1":"0");

    if(!p)
    {
        *val=0.0;
        return false;
    }

    if(!p->amp)
    {
        *val=0.0;
        return false;
    }
    return p->amp->getPWMLimit(off+p->base, val);
}
bool ControlBoardWrapper::setPWMLimit(int m, const double val)
{
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (!p->amp)
    {
        return false;
    }
    return p->amp->setPWMLimit(off+p->base, val);
}

bool ControlBoardWrapper::getPowerSupplyVoltage(int m, double* val)
{
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if(!p)
    {
        *val=0.0;
        return false;
    }

    if(!p->amp)
    {
        *val=0.0;
        return false;
    }
    return p->amp->getPowerSupplyVoltage(off+p->base, val);
}


/* IControlLimits */

bool ControlBoardWrapper::setLimits(int j, double min, double max)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->lim)
    {
        return p->lim->setLimits(off+p->base,min, max);
    }
    return false;
}

bool ControlBoardWrapper::getLimits(int j, double *min, double *max)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
    {
        *min=0.0;
        *max=0.0;
        return false;
    }

    if (p->lim)
    {
        return p->lim->getLimits(off+p->base,min, max);
    }
    *min=0.0;
    *max=0.0;
    return false;
}

bool ControlBoardWrapper::setVelLimits(int j, double min, double max)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (!p->lim)
    {
        return false;
    }
    return p->lim->setVelLimits(off+p->base,min, max);
}

bool ControlBoardWrapper::getVelLimits(int j, double *min, double *max)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    *min=0.0;
    *max=0.0;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
    {
        return false;
    }

    if(!p->lim)
    {
        return false;
    }
    return p->lim->getVelLimits(off+p->base,min, max);
}

/* IRemoteCalibrator */
IRemoteCalibrator *ControlBoardWrapper::getCalibratorDevice()
{
    yTrace();
    return yarp::dev::IRemoteCalibrator::getCalibratorDevice();
}

bool ControlBoardWrapper::isCalibratorDevicePresent(bool *isCalib)
{
    yTrace();
    return yarp::dev::IRemoteCalibrator::isCalibratorDevicePresent(isCalib);
}

bool ControlBoardWrapper::calibrateSingleJoint(int j)
{
    yTrace();
    if(!getCalibratorDevice())
        return false;
    return IRemoteCalibrator::getCalibratorDevice()->calibrateSingleJoint(j);
}

bool ControlBoardWrapper::calibrateWholePart()
{
    yTrace();
    if(!getCalibratorDevice())
        return false;

    return getCalibratorDevice()->calibrateWholePart();
}

bool ControlBoardWrapper::homingSingleJoint(int j)
{
    yTrace();
    if(!getCalibratorDevice())
        return false;

    return getCalibratorDevice()->homingSingleJoint(j);
}

bool ControlBoardWrapper::homingWholePart()
{
    yTrace();
    if(!getCalibratorDevice())
        return false;

    return getCalibratorDevice()->homingWholePart();
}

bool ControlBoardWrapper::parkSingleJoint(int j, bool _wait)
{
    yTrace();
    if(!getCalibratorDevice())
        return false;

    return getCalibratorDevice()->parkSingleJoint(j, _wait);
}

bool ControlBoardWrapper::parkWholePart()
{
    yTrace();
    if(!getCalibratorDevice())
        return false;

    return getCalibratorDevice()->parkWholePart();
}

bool ControlBoardWrapper::quitCalibrate()
{
    yTrace();
    if(!getCalibratorDevice())
        return false;

    return getCalibratorDevice()->quitCalibrate();
}

bool ControlBoardWrapper::quitPark()
{
    yTrace();
    if(!getCalibratorDevice())
        return false;

    return getCalibratorDevice()->quitPark();
}


/* IControlCalibration */
bool ControlBoardWrapper::calibrateAxisWithParams(int j, unsigned int ui, double v1, double v2, double v3)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p = device.getSubdevice(subIndex);
    if (p && p->calib)
    {
        return p->calib->calibrateAxisWithParams(off+p->base, ui,v1,v2,v3);
    }
    return false;
}

bool ControlBoardWrapper::setCalibrationParameters(int j, const CalibrationParameters& params)
{
    int off = device.lut[j].offset;
    int subIndex = device.lut[j].deviceEntry;

    SubDevice *p = device.getSubdevice(subIndex);
    if (p && p->calib)
    {
        return p->calib->setCalibrationParameters(off + p->base, params);
    }
    return false;
}

bool ControlBoardWrapper::calibrationDone(int j)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->calib)
    {
        return p->calib->calibrationDone(off+p->base);
    }
    return false;
}

bool ControlBoardWrapper::abortPark()
{
    yError("ControlBoardWrapper2::Calling abortPark -- not implemented\n");
    return false;
}

bool ControlBoardWrapper::abortCalibration()
{
    yError("ControlBoardWrapper2::Calling abortCalibration -- not implemented\n");
    return false;
}

/* IAxisInfo */

bool ControlBoardWrapper::getAxisName(int j, std::string& name)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->info)
    {
        return p->info->getAxisName(off+p->base, name);
    }
    return false;
}

bool ControlBoardWrapper::getJointType(int j, yarp::dev::JointTypeEnum& type)
{
    int off = device.lut[j].offset;
    int subIndex = device.lut[j].deviceEntry;

    SubDevice *p = device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->info)
    {
        return p->info->getJointType(off + p->base, type);
    }
    return false;
}

bool ControlBoardWrapper::getRefTorques(double *refs)
{
    auto* references = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iTorque) &&(ret = p->iTorque->getRefTorques(references)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                refs[juser] = references[jdevice];
            }
        }
        else
        {
            printError("getRefTorques", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] references;
    return ret;
}

bool ControlBoardWrapper::getRefTorque(int j, double *t)
{

    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->getRefTorque(off+p->base, t);
    }
    return false;
}

bool ControlBoardWrapper::setRefTorques(const double *t)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            ret=ret&&p->iTorque->setRefTorque(off+p->base, t[l]);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::setRefTorque(int j, double t)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->setRefTorque(off+p->base, t);
    }
    return false;
}

bool ControlBoardWrapper::setRefTorques(const int n_joints, const int *joints, const double *t)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for(int j=0; j<n_joints; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base;
        rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = t[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->iTorque)
        {
            ret= ret && rpcData.subdevices_p[subIndex]->iTorque->setRefTorques(rpcData.subdev_jointsVectorLen[subIndex],
                                                                               rpcData.jointNumbers[subIndex],
                                                                               rpcData.values[subIndex]);
        }
        else
        {
            ret=false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

bool ControlBoardWrapper::getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->getMotorTorqueParams(off+p->base, params);
    }
    return false;
}

bool ControlBoardWrapper::setMotorTorqueParams(int j,  const yarp::dev::MotorTorqueParameters params)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->setMotorTorqueParams(off+p->base, params);
    }
    return false;
}

bool ControlBoardWrapper::setImpedance(int j, double stiff, double damp)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iImpedance)
    {
        return p->iImpedance->setImpedance(off+p->base, stiff, damp);
    }

    return false;
}

bool ControlBoardWrapper::setImpedanceOffset(int j, double offset)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iImpedance)
    {
        return p->iImpedance->setImpedanceOffset(off+p->base, offset);
    }

    return false;
}

bool ControlBoardWrapper::getTorque(int j, double *t)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->getTorque(off+p->base, t);
    }

    return false;
}

bool ControlBoardWrapper::getTorques(double *t)
{
    auto* trqs = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iTorque) &&(ret = p->iTorque->getTorques(trqs)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                t[juser] = trqs[jdevice];
            }
        }
        else
        {
            printError("getTorques", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] trqs;
    return ret;

 }

bool ControlBoardWrapper::getTorqueRange(int j, double *min, double *max)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->getTorqueRange(off+p->base, min, max);
    }

    return false;
}

bool ControlBoardWrapper::getTorqueRanges(double *min, double *max)
{
    auto* t_min = new double[device.maxNumOfJointsInDevices];
    auto* t_max = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iTorque) &&(ret = p->iTorque->getTorqueRanges(t_min, t_max)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                min[juser] = t_min[jdevice];
                max[juser] = t_max[jdevice];
            }
        }
        else
        {
            printError("getTorqueRanges", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] t_min;
    delete [] t_max;
    return ret;

}

bool ControlBoardWrapper::getImpedance(int j, double* stiff, double* damp)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iImpedance)
    {
        return p->iImpedance->getImpedance(off+p->base, stiff, damp);
    }

    return false;
}

bool ControlBoardWrapper::getImpedanceOffset(int j, double* offset)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iImpedance)
    {
        return p->iImpedance->getImpedanceOffset(off+p->base, offset);
    }

    return false;
}

bool ControlBoardWrapper::getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iImpedance)
    {
        return p->iImpedance->getCurrentImpedanceLimit(off+p->base, min_stiff, max_stiff, min_damp, max_damp);
    }

    return false;
}

bool ControlBoardWrapper::getControlMode(int j, int *mode)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMode)
    {
        return p->iMode->getControlMode(off+p->base, mode);
    }
    return false;
}

bool ControlBoardWrapper::getControlModes(int *modes)
{
    int *all_mode = new int[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iMode) &&(ret = p->iMode->getControlModes(all_mode)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                modes[juser] = all_mode[jdevice];
            }
        }
        else
        {
            printError("getControlModes", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] all_mode;
    return ret;

}

// iControlMode2
bool ControlBoardWrapper::getControlModes(const int n_joint, const int *joints, int *modes)
{
    bool ret=true;

     for(int l=0; l<n_joint; l++)
     {
         int off=device.lut[joints[l]].offset;
         int subIndex=device.lut[joints[l]].deviceEntry;

         SubDevice *p=device.getSubdevice(subIndex);
         if (!p)
             return false;

         if (p->iMode)
         {
             ret=ret&&p->iMode->getControlMode(off+p->base, &modes[l]);
         }
         else
             ret=false;
     }
     return ret;
}

bool ControlBoardWrapper::setControlMode(const int j, const int mode)
{
    bool ret = true;
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMode)
    {
        ret = p->iMode->setControlMode(off+p->base, mode);
    }
    return ret;
}

bool ControlBoardWrapper::setControlModes(const int n_joints, const int *joints, int *modes)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for(int j=0; j<n_joints; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base;
        rpcData.modes[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = modes[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->iMode)
        {
            ret= ret && rpcData.subdevices_p[subIndex]->iMode->setControlModes(rpcData.subdev_jointsVectorLen[subIndex],
                                                                                rpcData.jointNumbers[subIndex],
                                                                                rpcData.modes[subIndex]);
        }
        else
        {
            ret=false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

bool ControlBoardWrapper::setControlModes(int *modes)
{
    bool ret = true;
    int j_wrap = 0;         // index of the wrapper joint

    int nDev = device.subdevices.size();
    for(int subDev_idx=0; subDev_idx < nDev; subDev_idx++)
    {
        int subIndex=device.lut[j_wrap].deviceEntry;
        SubDevice *p = device.getSubdevice(subIndex);
        if(!p) {
            return false;
        }

        int wrapped_joints=(p->top - p->base) + 1;
        int *joints = new int[wrapped_joints];

        if(p->iMode)
        {
            // versione comandi su subset di giunti
            for(int j_dev = 0; j_dev < wrapped_joints; j_dev++)
            {
                joints[j_dev] = p->base + j_dev;  // for all joints is equivalent to add offset term
            }

            ret = ret && p->iMode->setControlModes(wrapped_joints, joints, &modes[j_wrap]);
            j_wrap+=wrapped_joints;
        }

        if(joints!=nullptr)
        {
            delete [] joints;
            joints = nullptr;
        }
    }

    return ret;
}

bool ControlBoardWrapper::setPosition(int j, double ref)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->posDir)
    {
        return p->posDir->setPosition(off+p->base, ref);
    }

    return false;
}

bool ControlBoardWrapper::setPositions(const int n_joints, const int *joints, const double *dpos)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for(int j=0; j<n_joints; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        int offset = device.lut[joints[j]].offset;
        int base = rpcData.subdevices_p[subIndex]->base;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] =  offset + base;
        rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = dpos[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->posDir)   // Position Direct
        {
            ret= ret && rpcData.subdevices_p[subIndex]->posDir->setPositions(rpcData.subdev_jointsVectorLen[subIndex],
                                                                             rpcData.jointNumbers[subIndex],
                                                                             rpcData.values[subIndex]);
        }
        else
        {
            ret=false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

bool ControlBoardWrapper::setPositions(const double *refs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->posDir)
        {
            ret = p->posDir->setPosition(off+p->base, refs[l]) && ret;
        }
        else
            ret=false;
    }
    return ret;
}

yarp::os::Stamp ControlBoardWrapper::getLastInputStamp() {
    timeMutex.lock();
    yarp::os::Stamp ret=time;
    timeMutex.unlock();
    return ret;
}

bool ControlBoardWrapper::getRefPosition(const int j, double* ref)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);

    if (!p)
        return false;

    if (p->posDir)
    {
        bool ret = p->posDir->getRefPosition(off+p->base, ref);
        return ret;
    }
    *ref=0;
    return false;
}

bool ControlBoardWrapper::getRefPositions(double *spds)
{
    auto* references = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->posDir) &&(ret = p->posDir->getRefPositions(references)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                spds[juser] = references[jdevice];
            }
        }
        else
        {
            printError("getRefPositions", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] references;
    return ret;

}


bool ControlBoardWrapper::getRefPositions(const int n_joints, const int *joints, double *targets)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for(int j=0; j<n_joints; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base;
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->posDir)
        {
            ret= ret && rpcData.subdevices_p[subIndex]->posDir->getRefPositions( rpcData.subdev_jointsVectorLen[subIndex],
                                                                            rpcData.jointNumbers[subIndex],
                                                                            rpcData.values[subIndex]);
        }
    }

    if(ret)
    {
        // ReMix values by user expectations
        for(int i=0; i<rpcData.deviceNum; i++)
            rpcData.subdev_jointsVectorLen[i]=0;                  // reset tmp index

        // fill the output vector
        for(int j=0; j<n_joints; j++)
        {
            subIndex = device.lut[joints[j]].deviceEntry;
            targets[j]  = rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]];
            rpcData.subdev_jointsVectorLen[subIndex]++;
        }
    }
    else
    {
        for(int j=0; j<n_joints; j++)
        {
            targets[j] = 0;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}


//
// IVelocityControl2 Interface
//
bool ControlBoardWrapper::velocityMove(const int n_joints, const int *joints, const double *spds)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for(int j=0; j<n_joints; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base;
        rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = spds[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->vel)
        {
            ret= ret && rpcData.subdevices_p[subIndex]->vel->velocityMove(rpcData.subdev_jointsVectorLen[subIndex],
                                                                           rpcData.jointNumbers[subIndex],
                                                                           rpcData.values[subIndex]);
        }
        else
        {
            ret=false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

bool ControlBoardWrapper::getRefVelocity(const int j, double* vel)
{
    if(verbose())
        yTrace();

    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);

    if (!p)
        return false;

    if (p->vel)
    {
        bool ret = p->vel->getRefVelocity(off+p->base, vel);
        return ret;
    }
    *vel=0;
    return false;
}


bool ControlBoardWrapper::getRefVelocities(double* vels)
{
    auto* references = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->vel) &&(ret = p->vel->getRefVelocities(references)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                vels[juser] = references[jdevice];
            }
        }
        else
        {
            printError("getRefVelocities", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] references;
    return ret;

}

bool ControlBoardWrapper::getRefVelocities(const int n_joints, const int* joints, double* vels)
{
    if(verbose())
        yTrace();

    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for(int j=0; j<n_joints; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base;
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->vel)
        {
            ret= ret && rpcData.subdevices_p[subIndex]->vel->getRefVelocities( rpcData.subdev_jointsVectorLen[subIndex],
                                                                                rpcData.jointNumbers[subIndex],
                                                                                rpcData.values[subIndex]);
        }
    }

    if(ret)
    {
        // ReMix values by user expectations
        for(int i=0; i<rpcData.deviceNum; i++)
            rpcData.subdev_jointsVectorLen[i]=0;    // reset tmp index

        // fill the output vector
        for(int j=0; j<n_joints; j++)
        {
            subIndex = device.lut[joints[j]].deviceEntry;
            vels[j]  = rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]];
            rpcData.subdev_jointsVectorLen[subIndex]++;
        }
    }
    else
    {
        for(int j=0; j<n_joints; j++)
        {
            vels[j] = 0;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

bool ControlBoardWrapper::getInteractionMode(int j, yarp::dev::InteractionModeEnum* mode)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *s=device.getSubdevice(subIndex);
    if (!s)
        return false;

    if (s->iInteract)
    {
        return s->iInteract->getInteractionMode(off+s->base, mode);
    }
    return false;
}

bool ControlBoardWrapper::getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for(int j=0; j<n_joints; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base;
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->iInteract)
        {
            ret= ret && rpcData.subdevices_p[subIndex]->iInteract->getInteractionModes(rpcData.subdev_jointsVectorLen[subIndex],
                                                                                       rpcData.jointNumbers[subIndex],
                                                                                       (yarp::dev::InteractionModeEnum*) rpcData.modes[subIndex]);
        }
        else
        {
            ret=false;
        }
    }

    if(ret)
    {
        // ReMix values by user expectations
        for(int i=0; i<rpcData.deviceNum; i++)
            rpcData.subdev_jointsVectorLen[i]=0;                  // reset tmp index

        // fill the output vector
        for(int j=0; j<n_joints; j++)
        {
            subIndex = device.lut[joints[j]].deviceEntry;
            modes[j] = (yarp::dev::InteractionModeEnum) rpcData.modes[subIndex][rpcData.subdev_jointsVectorLen[subIndex]];
            rpcData.subdev_jointsVectorLen[subIndex]++;
        }
    }
    else
    {
        for(int j=0; j<n_joints; j++)
        {
            modes[j] = VOCAB_IM_UNKNOWN;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

bool ControlBoardWrapper::getInteractionModes(yarp::dev::InteractionModeEnum* modes)
{

    auto* imodes = new yarp::dev::InteractionModeEnum[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iInteract) &&(ret = p->iInteract->getInteractionModes(imodes)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                modes[juser] = imodes[jdevice];
            }
        }
        else
        {
            printError("getInteractionModes", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] imodes;
    return ret;
}

bool ControlBoardWrapper::setInteractionMode(int j, yarp::dev::InteractionModeEnum mode)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *s=device.getSubdevice(subIndex);
    if (!s)
        return false;

    if (s->iInteract)
    {
        return s->iInteract->setInteractionMode(off+s->base, mode);
    }
    return false;
}

bool ControlBoardWrapper::setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for(int j=0; j<n_joints; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base;
        rpcData.modes[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = (int) modes[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for(subIndex=0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if(rpcData.subdevices_p[subIndex]->iInteract)
        {
            ret= ret && rpcData.subdevices_p[subIndex]->iInteract->setInteractionModes( rpcData.subdev_jointsVectorLen[subIndex],
                                                                                        rpcData.jointNumbers[subIndex],
                                                                                        (yarp::dev::InteractionModeEnum*) rpcData.modes[subIndex]);
        }
        else
        {
            ret=false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

bool ControlBoardWrapper::setInteractionModes(yarp::dev::InteractionModeEnum* modes)
{
    bool ret = true;

    for(int j=0; j<controlledJoints; j++)
    {
        int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
        int subIndex=device.lut[j].deviceEntry;

        SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iInteract)
        {
            ret=ret && p->iInteract->setInteractionMode(off+p->base, modes[j]);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::setRefDutyCycle(int j, double v)
{
    int off; try{ off = device.lut.at(j).offset; }
    catch (...){ yError() << "joint number " << j << " out of bound [0-" << controlledJoints << "] for part " << partName; return false; }
    int subIndex = device.lut[j].deviceEntry;

    SubDevice *p = device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iPWM)
    {
        return p->iPWM->setRefDutyCycle(off + p->base, v);
    }
    return false;
}

bool ControlBoardWrapper::setRefDutyCycles(const double *v)
{
    bool ret = true;

    for (int l = 0; l<controlledJoints; l++)
    {
        int off = device.lut[l].offset;
        int subIndex = device.lut[l].deviceEntry;

        SubDevice *p = device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iPWM)
        {
            ret = ret&&p->iPWM->setRefDutyCycle(off + p->base, v[l]);
        }
        else
            ret = false;
    }
    return ret;
}

bool ControlBoardWrapper::getRefDutyCycle(int j, double *v)
{
    int off; try{ off = device.lut.at(j).offset; }
    catch (...){ yError() << "joint number " << j << " out of bound [0-" << controlledJoints << "] for part " << partName; return false; }
    int subIndex = device.lut[j].deviceEntry;

    SubDevice *p = device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iPWM)
    {
        return p->iPWM->getRefDutyCycle(off + p->base, v);
    }
    return false;
}

bool ControlBoardWrapper::getRefDutyCycles(double *v)
{
    auto* references = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iPWM) &&(ret = p->iPWM->getRefDutyCycles(references)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                v[juser] = references[jdevice];
            }
        }
        else
        {
            printError("getRefDutyCycles", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] references;
    return ret;

}

bool ControlBoardWrapper::getDutyCycle(int j, double *v)
{
    int off; try{ off = device.lut.at(j).offset; }
    catch (...){ yError() << "joint number " << j << " out of bound [0-" << controlledJoints << "] for part " << partName; return false; }
    int subIndex = device.lut[j].deviceEntry;

    SubDevice *p = device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iPWM)
    {
        return p->iPWM->getDutyCycle(off + p->base, v);
    }
    return false;
}

bool ControlBoardWrapper::getDutyCycles(double *v)
{
    auto* dutyCicles = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iPWM) &&(ret = p->iPWM->getDutyCycles(dutyCicles)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                v[juser] = dutyCicles[jdevice];
            }
        }
        else
        {
            printError("getDutyCycles", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] dutyCicles;
    return ret;

}


//
// ICurrentControl Interface
//

//bool ControlBoardWrapper::getAxes(int *ax);

bool ControlBoardWrapper::getCurrents(double *vals)
{
    auto* currs = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        ret = false;
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            break;
        }

        if(p->iCurr)
        {
            ret = p->iCurr->getCurrents(currs);
        }
        else if(p->amp)
        {
            ret = p->amp->getCurrents(currs);
        }

        if(ret)
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                vals[juser] = currs[jdevice];
            }
        }
        else
        {
            printError("getCurrents", p->id, ret);
            break;
        }
    }
    delete [] currs;
    return ret;
}

bool ControlBoardWrapper::getCurrent(int j, double *val)
{
    int off; try{off = device.lut.at(j).offset;} catch(...){yError() << "joint number " << j <<  " out of bound [0-"<< controlledJoints << "] for part " << partName; return false; }
    int subIndex=device.lut[j].deviceEntry;

    SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iCurr)
    {
        return p->iCurr->getCurrent(off+p->base,val);
    }
    else if (p->amp)
    {
        return p->amp->getCurrent(off+p->base,val);
    }
    *val=0.0;
    return false;
}

bool ControlBoardWrapper::getCurrentRange(int j, double *min, double *max)
{
    int off; try{ off = device.lut.at(j).offset; }
    catch (...){ yError() << "joint number " << j << " out of bound [0-" << controlledJoints << "] for part " << partName; return false; }
    int subIndex = device.lut[j].deviceEntry;

    SubDevice *p = device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iCurr)
    {
        return p->iCurr->getCurrentRange(off + p->base, min, max);
    }

    return false;
}

bool ControlBoardWrapper::getCurrentRanges(double *min, double *max)
{
    auto* c_min = new double[device.maxNumOfJointsInDevices];
    auto* c_max = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iCurr) &&(ret = p->iCurr->getCurrentRanges(c_min, c_max)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                min[juser] = c_min[jdevice];
                max[juser] = c_max[jdevice];
            }
        }
        else
        {
            printError("getCurrentRanges", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] c_min;
    delete [] c_max;
    return ret;

}

bool ControlBoardWrapper::setRefCurrents(const double *t)
{
    bool ret = true;

    for (int l = 0; l<controlledJoints; l++)
    {
        int off = device.lut[l].offset;
        int subIndex = device.lut[l].deviceEntry;

        SubDevice *p = device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iCurr)
        {
            ret = ret&&p->iCurr->setRefCurrent(off + p->base, t[l]);
        }
        else
            ret = false;
    }
    return ret;
}

bool ControlBoardWrapper::setRefCurrent(int j, double t)
{
    int off; try{ off = device.lut.at(j).offset; }
    catch (...){ yError() << "joint number " << j << " out of bound [0-" << controlledJoints << "] for part " << partName; return false; }
    int subIndex = device.lut[j].deviceEntry;

    SubDevice *p = device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iCurr)
    {
        return p->iCurr->setRefCurrent(off + p->base, t);
    }
    return false;
}

bool ControlBoardWrapper::setRefCurrents(const int n_joint, const int *joints, const double *t)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    int subIndex = 0;
    for (int j = 0; j<n_joint; j++)
    {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base;
        rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = t[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for (subIndex = 0; subIndex<rpcData.deviceNum; subIndex++)
    {
        if (rpcData.subdevices_p[subIndex]->iCurr)
        {
            ret = ret && rpcData.subdevices_p[subIndex]->iCurr->setRefCurrents(rpcData.subdev_jointsVectorLen[subIndex],
                rpcData.jointNumbers[subIndex],
                rpcData.values[subIndex]);
        }
        else
        {
            ret = false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

bool ControlBoardWrapper::getRefCurrents(double *t)
{
    auto* references = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for(unsigned int d=0; d<device.subdevices.size(); d++)
    {
        SubDevice *p=device.getSubdevice(d);
        if(!p)
        {
            ret = false;
            break;
        }

        if( (p->iCurr) &&(ret = p->iCurr->getRefCurrents(references)))
        {
            for(int juser= p->wbase, jdevice=p->base; juser<=p->wtop; juser++, jdevice++)
            {
                t[juser] = references[jdevice];
            }
        }
        else
        {
            printError("getRefCurrents", p->id, ret);
            ret = false;
            break;
        }
    }

    delete [] references;
    return ret;

}

bool ControlBoardWrapper::getRefCurrent(int j, double *t)
{
    int off; try{ off = device.lut.at(j).offset; }
    catch (...){ yError() << "joint number " << j << " out of bound [0-" << controlledJoints << "] for part " << partName; return false; }
    int subIndex = device.lut[j].deviceEntry;

    SubDevice *p = device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iCurr)
    {
        return p->iCurr->getRefCurrent(off + p->base, t);
    }

    return false;
}
