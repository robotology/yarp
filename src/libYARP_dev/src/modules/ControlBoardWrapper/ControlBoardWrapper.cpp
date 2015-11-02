// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include "ControlBoardWrapper.h"
#include "StreamingMessagesParser.h"
#include "RPCMessagesParser.h"
#include <iostream>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <string.h>         // for memset function

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::impl;
using namespace yarp::sig;
using namespace std;

// needed for the driver factory.
yarp::dev::DriverCreator *createControlBoardWrapper()
{
    return new yarp::dev::DriverCreatorOf<yarp::dev::ControlBoardWrapper>
            ("controlboardwrapper2", "controlboardwrapper2", "yarp::dev::ControlBoardWrapper2");
}

ControlBoardWrapper::ControlBoardWrapper() :yarp::os::RateThread(20),
                                            ownDevices(true)
{
    streaming_parser.init(this);
    RPC_parser.init(this);
    controlledJoints = 0;
    period = 20; // ms.
    base = 0;
    top = 0;
    subDeviceOwned = NULL;
    _verb = false;

    // init ROS data
    rosNodeName = "";
    rosTopicName = "";
    rosNode = NULL;
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

ControlBoardWrapper::~ControlBoardWrapper() { }

bool ControlBoardWrapper::close()
{
    //stop thread if running
    detachAll();

    if (yarp::os::RateThread::isRunning())
    {
        yarp::os::RateThread::stop();
    }

    if(useROS != ROS_only)
    {
        cleanup_yarpPorts();
    }

    if(rosNode != NULL)
    {
        delete rosNode;
        rosNode = NULL;
    }

    //if we own a deviced we have to close and delete it
    if (ownDevices)
    {
        // we should have created a new devices which we need to delete
        if(subDeviceOwned != NULL)
        {
            subDeviceOwned->close();
            delete subDeviceOwned;
            subDeviceOwned = NULL;
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
    else
    {
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
        yarp::os::ConstString ros_use_type = rosGroup.find("useROS").asString();
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
        // An error should be thrown later on in case we fail geting names from device
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

            if(nameList.size() != controlledJoints)
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
    yError() << partName << "should never get here!" << __LINE__;
    return false;  // should never get here
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

            if(rosNode == NULL)
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

            rootName = prop.check("rootName",Value("/"), "starting '/' if needed.").asString().c_str();
            partName=prop.check("name",Value("controlboard"), "prefix for port names").asString().c_str();
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

            if(!outputPositionStatePort.open((rootName+"/state:o").c_str()) )
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
    prop.fromString(config.toString().c_str());

    _verb = (prop.check("verbose","if present, give detailed output"));
    if (_verb)
        yInfo("ControlBoardWrapper: running with verbose output\n");

    if(!checkPortName(config) )
    {
        yError() << "the portName was not correctly set, check you r configuration file";
        return false;
    }

    bool found_period = false;

    // check FIRST for deprecated parameter
    if(prop.check("threadrate"))
    {
        yWarning() << " *** ControlBoardWrapper2 is using DEPRECATED parameter 'threadrate', use 'period' instead ***";
        if(!prop.find("threadrate").isInt())
        {
            yError() << " *** DEPRECATED 'threadrate' parameter is not an integer value, read value is " << period << " ***";
            return false;
        }
        period = prop.find("threadrate").asInt();
        if(period <= 0)
        {
            yError() << " *** DEPRECATED 'threadrate' parameter must be a positive value > 0 *** ";
            return false;
        }
        found_period = true;
    }

    // NOW, check for correct parameter, so if both are present we use the correct one
    if(prop.check("period"))
    {
        if(!prop.find("period").isInt())
        {
            yError() << " *** 'period' parameter is not an integer value *** ";
            return false;
        }
        if(found_period)
        {
            yWarning() << "*** ControlBoardWrapper2: found both 'period' and DEPRECATED 'threadrate' parameters. Using 'period', please remove the deprecated 'threadrate' param ***";
        }
        period = prop.find("period").asInt();
        if(period <= 0)
        {
            yError() << " *** 'period' parameter is not an integer value, read value is " << period << " ***";
            return false;
        }
        found_period = true;
    }

    // if none are present, fallback in the default value of 20ms set in the constructor
    if(!found_period)
    {
        yInfo() << "Using default 'period' of " << period << "ms";
    }

    // check if we need to create subdevice or if they are
    // passed later on thorugh attachAll()
    if(prop.check("subdevice"))
    {
        ownDevices=true;
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

    // call ROS node/topic initilization, if needed
    if(!initialize_ROS() )
    {
        return false;
    }

    // call YARP port initilization, if needed
    if(!initialize_YARP(prop) )
    {
        yError() << partName << "Something wrong when initting yarp ports";
        return false;
    }

    // In case attach is not deferred and the controlboard already owns a valid device
    // we can start the thread. Otherwise this will happen when attachAll is called
    if (ownDevices)
    {
       RateThread::setRate(period);
       RateThread::start();
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
    if(nets==0)
    {
       yError() <<"Error parsing parameters: \"networks\" should be followed by a list\n";
       return false;
    }

    if (!prop.check("joints", "number of joints of the part"))
        return false;

    controlledJoints=prop.find("joints").asInt();

    int nsubdevices=nets->size();
    device.lut.resize(controlledJoints);
    device.subdevices.resize(nsubdevices);

    // configure the devices
    int totalJ=0;
    for(int k=0;k<nets->size();k++)
    {
        Bottle parameters;
        int wBase;
        int wTop;

        parameters=prop.findGroup(nets->get(k).asString().c_str());

        if(parameters.size()==2)
        {
            Bottle *bot=parameters.get(1).asList();
            Bottle tmpBot;
            if(bot==NULL)
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

        SubDevice *tmpDevice=device.getSubdevice(k);
        tmpDevice->setVerbose(_verb);

        int axes=top-base+1;
        if (!tmpDevice->configure(base, top, axes, nets->get(k).asString().c_str()))
        {
            yError() <<"configure of subdevice ret false";
            return false;
        }

        for(int j=wBase;j<=wTop;j++)
        {
            device.lut[j].deviceEntry=k;
            device.lut[j].offset=j-wBase;
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
// open it and and attach to immediatly.
bool ControlBoardWrapper::openAndAttachSubDevice(Property& prop)
{
    Property p;
    subDeviceOwned = new PolyDriver;
    p.fromString(prop.toString().c_str());

    p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring
    p.unput("device");
    p.put("device",prop.find("subdevice").asString());  // subdevice was already checked before

    // if error occour during open, quit here.
    yDebug("opening controlBoardWrapper2 subdevice\n");
    subDeviceOwned->open(p);

    if (!subDeviceOwned->isValid())
    {
        yError("opening controlBoardWrapper2 subdevice... FAILED\n");
        return false;
    }

    yarp::dev::IEncoders * iencs = 0;

    subDeviceOwned->view(iencs);

    if (iencs == 0)
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

    SubDevice *tmpDevice=device.getSubdevice(0);
    tmpDevice->setVerbose(_verb);

    std::string subDevName ((partName + "_" + prop.find("subdevice").asString().c_str()));
    if (!tmpDevice->configure(base, top, controlledJoints, subDevName) )
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

    CBW_encoders.resize(device.lut.size());

    // initialization.
    RPC_parser.initialize();
    updateAxisName();
    return true;
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

    yarp::os::ConstString tmp;
    // I need a temporary vector because if I'm wrapping more than one subdevice, and one of them
    // does not have the axesName, then I'd stick with the old names from wrpper config file, if any.
    vector<string> tmpVect;
    bool ret = true;

    tmpVect.clear();
    for(int i=0; i < controlledJoints; i++)
    {
        if( (ret = getAxisName(i, tmp) && ret) )
        {
            std::string tmp2(tmp.c_str());
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
        std::string tmpKey=polylist[p]->key.c_str();
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
    for(unsigned int k=0; k<device.subdevices.size(); k++)
    {
        if (!device.subdevices[k].isAttached())
        {
            ready=false;
        }
    }

    if (!ready)
    {
        yError("ControlBoardWrapper: AttachAll failed, some subdevice was not found or its attach failed\n");
        return false;
    }

    CBW_encoders.resize(device.lut.size());

    // initialization.
    RPC_parser.initialize();

    updateAxisName();
    RateThread::setRate(period);
    RateThread::start();

    return true;
}

bool ControlBoardWrapper::detachAll()
{
        //check if we already instantiated a subdevice previously
        if (ownDevices)
            return false;

        if (yarp::os::RateThread::isRunning())
            yarp::os::RateThread::stop();

        int devices=device.subdevices.size();
        for(int k=0;k<devices;k++)
            device.getSubdevice(k)->detach();

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

    if(useROS != ROS_only)
    {
        yarp::sig::Vector& v = outputPositionStatePort.prepare();
        v.size(controlledJoints);

        //getEncoders for all subdevices
        double *joint_encoders=v.data();
        double joint_timeStamp=0.0;

        for(unsigned int k=0;k<device.subdevices.size();k++)
        {
            int axes=device.subdevices[k].axes;

            device.subdevices[k].refreshJointEncoders();
            device.subdevices[k].refreshMotorEncoders();

            for(int l=0;l<axes;l++)
            {
                joint_encoders[l]=device.subdevices[k].subDev_joint_encoders[l];
                joint_timeStamp+=device.subdevices[k].jointEncodersTimes[l];
            }
            joint_encoders+=device.subdevices[k].axes; //jump to next group
        }

        //joint_timeStamp =  yarp::os::Time::now();
        timeMutex.wait();
        time.update(joint_timeStamp/controlledJoints);
        timeMutex.post();

        outputPositionStatePort.setEnvelope(time);
        outputPositionStatePort.write();

        jointData &yarp_struct = extendedOutputState_buffer.get();

        yarp_struct.jointPosition.resize(controlledJoints);
        yarp_struct.jointVelocity.resize(controlledJoints);
        yarp_struct.jointAcceleration.resize(controlledJoints);
        yarp_struct.motorPosition.resize(controlledJoints);
        yarp_struct.motorVelocity.resize(controlledJoints);
        yarp_struct.motorAcceleration.resize(controlledJoints);
        yarp_struct.torque.resize(controlledJoints);
        yarp_struct.pidOutput.resize(controlledJoints);
        yarp_struct.controlMode.resize(controlledJoints);
        yarp_struct.interactionMode.resize(controlledJoints);

        yarp_struct.jointPosition_isValid       = getEncoders(yarp_struct.jointPosition.data());
        yarp_struct.jointVelocity_isValid       = getEncoderSpeeds(yarp_struct.jointVelocity.data());
        yarp_struct.jointAcceleration_isValid   = getEncoderAccelerations(yarp_struct.jointAcceleration.data());
        yarp_struct.motorPosition_isValid       = getMotorEncoders(yarp_struct.motorPosition.data());
        yarp_struct.motorVelocity_isValid       = getMotorEncoderSpeeds(yarp_struct.motorVelocity.data());
        yarp_struct.motorAcceleration_isValid   = getMotorEncoderAccelerations(yarp_struct.motorAcceleration.data());
        yarp_struct.torque_isValid              = getTorques(yarp_struct.torque.data());
        yarp_struct.pidOutput_isValid           = getOutputs(yarp_struct.pidOutput.data());
        yarp_struct.controlMode_isValid         = getControlModes(yarp_struct.controlMode.data());
        yarp_struct.interactionMode_isValid     = getInteractionModes((yarp::dev::InteractionModeEnum* ) yarp_struct.interactionMode.data());

        extendedOutputStatePort.setEnvelope(time);
        extendedOutputState_buffer.write();
    }

    if(useROS != ROS_disabled)
    {
        sensor_msgs_JointState ros_struct;

        ros_struct.name.resize(controlledJoints);
        ros_struct.position.resize(controlledJoints);
        ros_struct.velocity.resize(controlledJoints);
        ros_struct.effort.resize(controlledJoints);

        getEncoders(ros_struct.position.data());
        getEncoderSpeeds(ros_struct.velocity.data());
        getTorques(ros_struct.effort.data());

        convertDegreesToRadians(ros_struct.position);
        convertDegreesToRadians(ros_struct.velocity);
        ros_struct.name=jointNames;

        ros_struct.header.seq = rosMsgCounter++;
        ros_struct.header.stamp = normalizeSecNSec(yarp::os::Time::now());

        rosPublisherPort.write(ros_struct);
    }
}

//
//  IPid Interface
//
bool ControlBoardWrapper::setPid(int j, const Pid &p)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *s=device.getSubdevice(subIndex);
    if (!s)
        return false;

    if (s->pid)
    {
        return s->pid->setPid(off+s->base, p);
    }
    return false;
}

bool ControlBoardWrapper::setPids(const Pid *ps)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            ret=ret&&p->pid->setPid(off+p->base, ps[l]);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::setReference(int j, double ref)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->setReference(off+p->base, ref);
    }
    return false;
}

bool ControlBoardWrapper::setReferences(const double *refs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            ret=ret&&p->pid->setReference(off+p->base, refs[l]);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::setErrorLimit(int j, double limit)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->setErrorLimit(off+p->base, limit);
    }
    return false;
}

/** Get the error limit for the controller on all joints.
* @param limits pointer to the vector with the new limits
* @return true/false on success/failure
*/
bool ControlBoardWrapper::setErrorLimits(const double *limits)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            ret=ret&&p->pid->setErrorLimit(off+p->base, limits[l]);
        }
        else
            ret=false;
    }
    return ret;
}

/** Get the current error for a joint.
* @param j joint number
* @param err pointer to the storage for the return value
* @return true/false on success failure
*/
bool ControlBoardWrapper::getError(int j, double *err)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->getError(off+p->base, err);
    }
    *err = 0.0;
    return false;
}

/** Get the error of all joints.
* @param errs pointer to the vector that will store the errors.
* @return true/false on success/failure.
*/
bool ControlBoardWrapper::getErrors(double *errs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            ret=ret&&p->pid->getError(off+p->base, errs+l);
        }
        else
            ret=false;
    }
    return ret;
}

/** Get the output of the controller (e.g. pwm value)
* @param j joint number
* @param out pointer to storage for return value
* @return success/failure
*/
bool ControlBoardWrapper::getOutput(int j, double *out)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->getOutput(off+p->base, out);
    }
    *out=0.0;
    return false;
}

/** Get the output of the controllers (e.g. pwm value)
* @param outs pinter to the vector that will store the output values
*/
bool ControlBoardWrapper::getOutputs(double *outs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            ret=ret&&p->pid->getOutput(off+p->base, outs+l);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::setOffset(int j, double v)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->setOffset(off+p->base, v);
    }
    return false;
}

/** Get current pid value for a specific joint.
* @param j joint number
* @param p pointer to storage for the return value.
* @return success/failure
*/
bool ControlBoardWrapper::getPid(int j, Pid *p)
{
//#warning "check for max number of joints!?!?!"
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *s=device.getSubdevice(subIndex);
    if (!s)
        return false;

    if (s->pid)
    {
        return s->pid->getPid(off+s->base, p);
    }
    return false;
}

/** Get current pid value for a specific joint.
* @param pids vector that will store the values of the pids.
* @return success/failure
*/
bool ControlBoardWrapper::getPids(Pid *pids)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            ret=ret&&p->pid->getPid(off+p->base, pids+l);
        }
        else
            ret=false;
    }
    return ret;
}

/** Get the current reference position of the controller for a specific joint.
* @param j joint number
* @param ref pointer to storage for return value
* @return reference value
*/
bool ControlBoardWrapper::getReference(int j, double *ref) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;
    if (p->pid)
    {
        return p->pid->getReference(off+p->base, ref);
    }
    return false;
}

/** Get the current reference position of all controllers.
* @param refs vector that will store the output.
*/
bool ControlBoardWrapper::getReferences(double *refs) {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            ret=ret&&p->pid->getReference(off+p->base, refs+l);
        }
        else
            ret=false;
    }
    return ret;
}

/** Get the error limit for the controller on a specific joint
* @param j joint number
* @param limit pointer to storage
* @return success/failure
*/
bool ControlBoardWrapper::getErrorLimit(int j, double *limit) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->getErrorLimit(off+p->base, limit);
    }
    return false;
}

/** Get the error limit for all controllers
* @param limits pointer to the array that will store the output
* @return success or failure
*/
bool ControlBoardWrapper::getErrorLimits(double *limits) {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->pid)
        {
            ret=ret&&p->pid->getErrorLimit(off+p->base, limits+l);
        }
        else
            ret=false;
    }
    return ret;
}

/** Reset the controller of a given joint, usually sets the
* current position of the joint as the reference value for the PID, and resets
* the integrator.
* @param j joint number
* @return true on success, false on failure.
*/
bool ControlBoardWrapper::resetPid(int j) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->resetPid(off+p->base);
    }
    return false;
}

/**
* Disable the pid computation for a joint
* @param j is the axis number
* @return true if successful, false on failure
**/
bool ControlBoardWrapper::disablePid(int j) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    // Use the newer interface if available, otherwise fallback on the old one.
    if(p->iMode2)
        return p->iMode2->setControlMode(off+p->base, VOCAB_CM_IDLE);
    else
        if (p->pid)
        {
            return p->pid->disablePid(off+p->base);
        }
    return false;
}

/**
* Enable the pid computation for a joint
* @param j is the axis number
* @return true/false on success/failure
*/
bool ControlBoardWrapper::enablePid(int j) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->pid)
    {
        return p->pid->enablePid(off+p->base);
    }
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
* Set position mode. This command
* is required by control boards implementing different
* control methods (e.g. velocity/torque), in some cases
* it can be left empty.
* return true/false on success/failure
*/
bool ControlBoardWrapper::setPositionMode() {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

        if (!p)
            return false;

        if (p->iMode2)
        {
            yDebug() << "setPositionMode() for ALL joint using NEW interface";

            //calling new iControlMode2 interface
            ret = ret && p->iMode2->setControlMode(off+p->base, VOCAB_CM_POSITION);
        }
        else if(p->iMode)
        {
            yDebug() << "setPositionMode() for ALL joint using OLD interface";

            //calling old iControlMode interface
            ret=ret&&p->iMode->setPositionMode(off+p->base);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::setOpenLoopMode() {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

        if (!p)
            return false;

        if (p->iMode2)
        {
            yDebug() << "setOpenLoopMode() for ALL joint using NEW interface";

            //calling new iControlMode2 interface
            ret = ret && p->iMode2->setControlMode(off+p->base, VOCAB_CM_OPENLOOP);
        }
        else if(p->iMode)
        {
            yDebug() << "setOpenLoopMode() for ALL joint using OLD interface";

            //calling iControlMode interface
            ret=ret&&p->iMode->setOpenLoopMode(off+p->base);
        }
        else
            ret=false;
    }
    return ret;
}

/**
* Set new reference point for a single axis.
* @param j joint number
* @param ref specifies the new ref point
* @return true/false on success/failure
*/
bool ControlBoardWrapper::positionMove(int j, double ref) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

        int wrapped_joints=(p->top - p->base) + 1;
        int *joints = new int[wrapped_joints];

        if(!p)
            return false;

        if(p->pos2)   // Position Control 2
        {
            // versione comandi su subset di giunti
            for(int j_dev = 0; j_dev < wrapped_joints; j_dev++)
            {
                joints[j_dev] = p->base + j_dev;  // for all joints is equivalent to add offset term
            }

            ret = ret && p->pos2->positionMove(wrapped_joints, joints, &refs[j_wrap]);
            j_wrap+=wrapped_joints;
        }
        else   // Classic Position Control
        {
            if(p->pos)
            {

                for(int j_dev = 0; j_dev < wrapped_joints; j_dev++, j_wrap++)
                {
                    int off=device.lut[j_wrap].offset;
                    ret=ret && p->pos->positionMove(p->base+off, refs[j_wrap]);
                }
            }
            else
            {
                ret=false;
            }
        }

        if(joints!=0)
        { delete [] joints;
          joints = 0;}
    }

    return ret;
}

/** Set new reference point for a subset of axis.
 * @param joints pointer to the array of joint numbers
 * @param refs   pointer to the array specifing the new reference points
 * @return true/false on success/failure
 */
bool ControlBoardWrapper::positionMove(const int n_joints, const int *joints, const double *refs)
{
    bool ret = true;

    rpcDataMutex.wait();
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
        if(rpcData.subdevices_p[subIndex]->pos2)   // Position Control 2
        {
            ret= ret && rpcData.subdevices_p[subIndex]->pos2->positionMove(rpcData.subdev_jointsVectorLen[subIndex],
                                                                           rpcData.jointNumbers[subIndex],
                                                                           rpcData.values[subIndex]);
        }
        else   // Classic Position Control
        {
            if(rpcData.subdevices_p[subIndex]->pos)
            {
                for(int i = 0; i < rpcData.subdev_jointsVectorLen[subIndex]; i++)
                {
                    ret=ret && rpcData.subdevices_p[subIndex]->pos->positionMove(rpcData.jointNumbers[subIndex][i],
                                                                                 rpcData.values[subIndex][i]);
                }
            }
            else
            {
                ret=false;
            }
        }
    }
    rpcDataMutex.post();
    return ret;
}

/** Set relative position. The command is relative to the
* current position of the axis.
* @param j joint axis number
* @param delta relative command
* @return true/false on success/failure
*/
bool ControlBoardWrapper::relativeMove(int j, double delta) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

    rpcDataMutex.wait();
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
        if(rpcData.subdevices_p[subIndex]->pos2)   // Position Control 2
        {
            ret= ret && rpcData.subdevices_p[subIndex]->pos2->relativeMove(rpcData.subdev_jointsVectorLen[subIndex],
                                                                           rpcData.jointNumbers[subIndex],
                                                                           rpcData.values[subIndex]);
        }
        else   // Classic Position Control
        {
            if(rpcData.subdevices_p[subIndex]->pos)
            {
                for(int i = 0; i < rpcData.subdev_jointsVectorLen[subIndex]; i++)
                {
                    ret=ret && rpcData.subdevices_p[subIndex]->pos->relativeMove(rpcData.jointNumbers[subIndex][i],
                                                                                 rpcData.values[subIndex][i]);
                }
            }
            else
            {
                ret=false;
            }
        }
    }
    rpcDataMutex.post();
    return ret;
}

/**
* Check if the current trajectory is terminated. Non blocking.
* @param j the axis
* @param flag true if the trajectory is terminated, false otherwise
* @return false on failure
*/
bool ControlBoardWrapper::checkMotionDone(int j, bool *flag) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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
bool ControlBoardWrapper::checkMotionDone(bool *flag) {
    bool ret=true;
    *flag=true;
    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

        if (!p)
            return false;

        if (p->pos)
        {
            bool tmpF=false;
            ret=ret&&p->pos->checkMotionDone(off+p->base, &tmpF);
            *flag=*flag&&tmpF;
        }
        else
            ret=false;
    }
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
    bool   XFlags = true;

   rpcDataMutex.wait();
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
       if(rpcData.subdevices_p[subIndex]->pos2)   // Position Control 2
       {
           ret= ret && rpcData.subdevices_p[subIndex]->pos2->checkMotionDone(rpcData.subdev_jointsVectorLen[subIndex],
                                                                             rpcData.jointNumbers[subIndex],
                                                                             &XFlags);
           *flags = flags && XFlags;
       }
       else   // Classic Position Control
       {
           if(rpcData.subdevices_p[subIndex]->pos)
           {
               for(int i = 0; i < rpcData.subdev_jointsVectorLen[subIndex]; i++)
               {
                   ret=ret && rpcData.subdevices_p[subIndex]->pos->checkMotionDone(rpcData.jointNumbers[subIndex][i],
                                                                                   &XFlags);
                   *flags = flags && XFlags;
               }
           }
           else
           {
               ret=false;
           }
       }
   }
    return ret;
}

/** Set reference speed for a joint, this is the speed used during the
* interpolation of the trajectory.
* @param j joint number
* @param sp speed value
* @return true/false upon success/failure
*/
bool ControlBoardWrapper::setRefSpeed(int j, double sp) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
        yarp::dev::impl::SubDevice *p=device.getSubdevice(subDev_idx);

        if(!p)
            return false;

        int wrapped_joints=(p->top - p->base) + 1;
        int *joints = new int[wrapped_joints];

        if(p->pos2)   // Position Control 2
        {
            // verione comandi su subset di giunti
            for(int j_dev = 0; j_dev < wrapped_joints; j_dev++)
            {
                joints[j_dev] = p->base + j_dev;
            }

            p->pos2->setRefSpeeds(wrapped_joints, joints, &spds[j_wrap]);
            j_wrap += wrapped_joints;
        }
        else   // Classic Position Control
        {
            if(p->pos)
            {
                for(int j_dev = 0; j_dev < wrapped_joints; j_dev++, j_wrap++)
                {
                    int off=device.lut[j_wrap].offset;
                    ret=ret && p->pos->setRefSpeed(p->base+off, spds[j_wrap]);
                }
            }
            else
            {
                ret=false;
            }
        }

        if(joints!=0)
        { delete [] joints;
          joints = 0;}
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

    rpcDataMutex.wait();
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
        if(rpcData.subdevices_p[subIndex]->pos2)   // Position Control 2
        {
            ret= ret && rpcData.subdevices_p[subIndex]->pos2->setRefSpeeds( rpcData.subdev_jointsVectorLen[subIndex],
                                                                            rpcData.jointNumbers[subIndex],
                                                                            rpcData.values[subIndex]);
        }
        else   // Classic Position Control
        {
            if(rpcData.subdevices_p[subIndex]->pos)
            {
                for(int i = 0; i < rpcData.subdev_jointsVectorLen[subIndex]; i++)
                {
                    ret=ret && rpcData.subdevices_p[subIndex]->pos->setRefSpeed(rpcData.jointNumbers[subIndex][i],
                                                                                rpcData.values[subIndex][i]);
                }
            }
            else
            {
                ret=false;
            }
        }
    }
    rpcDataMutex.post();
    return ret;
}

/** Set reference acceleration for a joint. This value is used during the
* trajectory generation.
* @param j joint number
* @param acc acceleration value
* @return true/false upon success/failure
*/
bool ControlBoardWrapper::setRefAcceleration(int j, double acc) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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
        yarp::dev::impl::SubDevice *p=device.getSubdevice(subDev_idx);

        if(!p)
            return false;

        int wrapped_joints=(p->top - p->base) + 1;
        int *joints = new int[wrapped_joints];  // to be defined once and for all?

        if(p->pos2)   // Position Control 2
        {
            // verione comandi su subset di giunti
            for(int j_dev = 0; j_dev < wrapped_joints; j_dev++)
            {
                joints[j_dev] = p->base + j_dev;
            }

            p->pos2->setRefAccelerations(wrapped_joints, joints, &accs[j_wrap]);
            j_wrap += wrapped_joints;
        }
        else        // Classic Position Control
        {
            if(p->pos)
            {
                for(int j_dev = 0; j_dev < wrapped_joints; j_dev++, j_wrap++)
                {
                    int off=device.lut[j_wrap].offset;
                    ret=ret && p->pos->setRefAcceleration(p->base+off, accs[j_wrap]);
                }
            }
            else
            {
                ret=false;
            }
        }

        if(joints!=0)
        { delete [] joints;
        joints = 0;}
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

    rpcDataMutex.wait();
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
        if(rpcData.subdevices_p[subIndex]->pos2)   // Position Control 2
        {
            ret= ret && rpcData.subdevices_p[subIndex]->pos2->setRefAccelerations(  rpcData.subdev_jointsVectorLen[subIndex],
                                                                                    rpcData.jointNumbers[subIndex],
                                                                                    rpcData.values[subIndex]);
        }
        else   // Classic Position Control
        {
            if(rpcData.subdevices_p[subIndex]->pos)
            {
                for(int i = 0; i < rpcData.subdev_jointsVectorLen[subIndex]; i++)
                {
                    ret=ret && rpcData.subdevices_p[subIndex]->pos->setRefAcceleration(rpcData.jointNumbers[subIndex][i],
                                                                                       rpcData.values[subIndex][i]);
                }
            }
            else
            {
                ret=false;
            }
        }
    }
    rpcDataMutex.post();
    return ret;
}


/** Get reference speed for a joint. Returns the speed used to
 * generate the trajectory profile.
 * @param j joint number
 * @param ref pointer to storage for the return value
 * @return true/false on success or failure
 */
bool ControlBoardWrapper::getRefSpeed(int j, double *ref) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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
bool ControlBoardWrapper::getRefSpeeds(double *spds) {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

        if (!p)
            return false;

        if (p->pos)
        {
            ret=ret&&p->pos->getRefSpeed(off+p->base, spds+l);
        }
        else
            ret=false;
    }
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

    rpcDataMutex.wait();
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
        if(rpcData.subdevices_p[subIndex]->pos2)   // Position Control 2
        {
            ret= ret && rpcData.subdevices_p[subIndex]->pos2->getRefSpeeds( rpcData.subdev_jointsVectorLen[subIndex],
                                                                            rpcData.jointNumbers[subIndex],
                                                                            rpcData.values[subIndex]);
        }
        else   // Classic Position Control
        {
            if(rpcData.subdevices_p[subIndex]->pos)
            {
                for(int i = 0; i < rpcData.subdev_jointsVectorLen[subIndex]; i++)
                {
                    ret=ret && rpcData.subdevices_p[subIndex]->pos->getRefSpeed( rpcData.jointNumbers[subIndex][i],
                                                                                &rpcData.values[subIndex][i]);
                }
            }
            else
            {
                ret=false;
            }
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
    rpcDataMutex.post();
    return ret;
}

/** Get reference acceleration for a joint. Returns the acceleration used to
* generate the trajectory profile.
* @param j joint number
* @param acc pointer to storage for the return value
* @return true/false on success/failure
*/
bool ControlBoardWrapper::getRefAcceleration(int j, double *acc) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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
bool ControlBoardWrapper::getRefAccelerations(double *accs) {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

        if (!p)
            return false;

        if (p->pos)
        {
            ret=ret&&p->pos->getRefAcceleration(off+p->base, accs+l);
        }
        else
            ret=false;
    }
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

    rpcDataMutex.wait();
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
        if(rpcData.subdevices_p[subIndex]->pos2)   // Position Control 2
        {
            ret= ret && rpcData.subdevices_p[subIndex]->pos2->getRefAccelerations(  rpcData.subdev_jointsVectorLen[subIndex],
                                                                                    rpcData.jointNumbers[subIndex],
                                                                                    rpcData.values[subIndex]);
        }
        else   // Classic Position Control
        {
            if(rpcData.subdevices_p[subIndex]->pos)
            {
                for(int i = 0; i < rpcData.subdev_jointsVectorLen[subIndex]; i++)
                {
                    ret=ret && rpcData.subdevices_p[subIndex]->pos->getRefAcceleration( rpcData.jointNumbers[subIndex][i],
                                                                                       &rpcData.values[subIndex][i]);
                }
            }
            else
            {
                ret=false;
            }
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
    rpcDataMutex.post();
    return ret;
}

/** Stop motion, single joint
* @param j joint number
* @return true/false on success/failure
*/
bool ControlBoardWrapper::stop(int j) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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

    rpcDataMutex.wait();
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
        if(rpcData.subdevices_p[subIndex]->pos2)   // Position Control 2
        {
            ret= ret && rpcData.subdevices_p[subIndex]->pos2->stop(rpcData.subdev_jointsVectorLen[subIndex],
                                                                   rpcData.jointNumbers[subIndex]);
        }
        else   // Classic Position Control
        {
            if(rpcData.subdevices_p[subIndex]->pos)
            {
                for(int i = 0; i < rpcData.subdev_jointsVectorLen[subIndex]; i++)
                {
                    ret=ret && rpcData.subdevices_p[subIndex]->pos->stop(rpcData.jointNumbers[subIndex][i]);
                }
            }
            else
            {
                ret=false;
            }
        }
    }
    rpcDataMutex.post();
    return ret;
}


/* IVelocityControl */

bool ControlBoardWrapper::velocityMove(int j, double v) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

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
        yarp::dev::impl::SubDevice *p=device.getSubdevice(subDev_idx);

        if(!p)
            return false;

        int wrapped_joints=(p->top - p->base) + 1;
        int *joints = new int[wrapped_joints];

        if(p->vel2)   // Velocity Control 2
        {
            // verione comandi su subset di giunti
            for(int j_dev = 0; j_dev < wrapped_joints; j_dev++)
            {
                joints[j_dev] = p->base + j_dev;
            }

            ret = ret && p->vel2->velocityMove(wrapped_joints, joints, &v[j_wrap]);
            j_wrap += wrapped_joints;
        }
        else   // Classic Position Control
        {
            if(p->vel)
            {
                for(int j_dev = 0; j_dev < wrapped_joints; j_dev++, j_wrap++)
                {
                    int off=device.lut[j_wrap].offset;
                    ret=ret && p->vel->velocityMove(p->base+off, v[j_wrap]);
                }
            }
            else
            {
                ret=false;
            }
        }

        if(joints!=0)
        { delete [] joints;
          joints = 0;}
    }

    return ret;
}

bool ControlBoardWrapper::setVelocityMode()
{
    bool ret=true;
    int j_wrap = 0;         // index of the wrapper joint
    for(unsigned int subDev_idx=0; subDev_idx < device.subdevices.size(); subDev_idx++)
    {
        yarp::dev::impl::SubDevice *p=device.getSubdevice(subDev_idx);

        if(!p)
            return false;

        int wrapped_joints=(p->top - p->base) + 1;

        if(p->iMode2)   // ControlMode2
        {
            for(int j_dev = 0; j_dev < wrapped_joints; j_dev++, j_wrap++)
            {   ret = ret && p->iMode2->setControlMode(p->base + j_dev, VOCAB_CM_VELOCITY);   }
        }
        else if(p->iMode)
        {
            for(int j_dev = 0; j_dev < wrapped_joints; j_dev++, j_wrap++)
            {
                int off=device.lut[j_wrap].offset;
                ret = ret && p->iMode->setVelocityMode(p->base+off);
            }
        }
    }
    return ret;
}

/* IEncoders */

bool ControlBoardWrapper::resetEncoder(int j) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iJntEnc)
    {
        return p->iJntEnc->getEncoder(off+p->base, v);
    }
    *v=0.0;
    return false;
}

bool ControlBoardWrapper::getEncoders(double *encs) {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iJntEnc)
        {
            ret=ret&&p->iJntEnc->getEncoder(off+p->base, encs+l);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getEncodersTimed(double *encs, double *t) {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iJntEnc)
        {
            ret=ret&&p->iJntEnc->getEncoderTimed(off+p->base, encs+l, t+l);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getEncoderTimed(int j, double *v, double *t) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iJntEnc)
    {
        return p->iJntEnc->getEncoderSpeed(off+p->base, sp);
    }
    *sp=0.0;
    return false;
}

bool ControlBoardWrapper::getEncoderSpeeds(double *spds) {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iJntEnc)
        {
            ret=ret&&p->iJntEnc->getEncoderSpeed(off+p->base, spds+l);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getEncoderAcceleration(int j, double *acc) {
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iJntEnc)
        {
            ret=ret&&p->iJntEnc->getEncoderAcceleration(off+p->base, accs+l);
        }
        else
            ret=false;
    }
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

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->imotor)
    {
        return p->imotor->getTemperature(off+p->base, val);
    }
    *val=0.0;
    return false;
}

bool ControlBoardWrapper::getTemperatures     (double *vals) {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->imotor)
        {
            ret=ret&&p->imotor->getTemperature(off+p->base, vals+l);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getTemperatureLimit (int m, double* val) {
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

    yarp::dev::impl::SubDevice *p = device.getSubdevice(subIndex);
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

    yarp::dev::impl::SubDevice *p = device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->imotor)
    {
        return p->imotor->setGearboxRatio(off + p->base, val);
    }
    return false;
}

bool ControlBoardWrapper::getMotorOutputLimit (int m, double* val) {
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->imotor)
    {
        return p->imotor->getMotorOutputLimit(off+p->base, val);
    }
    *val=0.0;
    return false;
}

bool ControlBoardWrapper::setMotorOutputLimit (int m, const double val) {
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->imotor)
    {
        return p->imotor->setMotorOutputLimit(off+p->base,val);
    }
    return false;
}

/* IRemoteVariables */
bool ControlBoardWrapper::getRemoteVariable(yarp::os::ConstString key, yarp::os::Bottle& val) {
    bool b = true;

    for (unsigned int i = 0; i < device.subdevices.size(); i++)
    {
        yarp::dev::impl::SubDevice *p = device.getSubdevice(i);

        if (!p) return false;
        if (!p->iVar) return false;
        yarp::os::Bottle tmpval;
        b &= p->iVar->getRemoteVariable(key, tmpval);
        if (b) val.append(tmpval);
    }

    return b;
}

bool ControlBoardWrapper::setRemoteVariable(yarp::os::ConstString key, const yarp::os::Bottle& val) {
    //int off = device.lut[0].offset;
    int subIndex = device.lut[0].deviceEntry;
    yarp::dev::impl::SubDevice *p = device.getSubdevice(subIndex);

    if (!p)
    return false;

    if (p->iVar)
    {
    return p->iVar->setRemoteVariable(key, val);
    }

    return false;
}

bool ControlBoardWrapper::getRemoteVariablesList(yarp::os::Bottle* listOfKeys)
{
    //int off = device.lut[0].offset;
    int subIndex = device.lut[0].deviceEntry;
    yarp::dev::impl::SubDevice *p = device.getSubdevice(subIndex);

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

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoder(off+p->base, v);
    }
    *v=0.0;
    return false;
}

bool ControlBoardWrapper::getMotorEncoders(double *encs) {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iMotEnc)
        {
            ret=ret&&p->iMotEnc->getMotorEncoder(off+p->base, encs+l);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getMotorEncodersTimed(double *encs, double *t) {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iMotEnc)
        {
            ret=ret&&p->iMotEnc->getMotorEncoderTimed(off+p->base, encs, t);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getMotorEncoderTimed(int m, double *v, double *t) {
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMotEnc)
    {
        return p->iMotEnc->getMotorEncoderSpeed(off+p->base, sp);
    }
    *sp=0.0;
    return false;
}

bool ControlBoardWrapper::getMotorEncoderSpeeds(double *spds) {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iMotEnc)
        {
            ret=ret&&p->iMotEnc->getMotorEncoderSpeed(off+p->base, spds+l);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getMotorEncoderAcceleration(int m, double *acc) {
    int off=device.lut[m].offset;
    int subIndex=device.lut[m].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iMotEnc)
        {
            ret=ret&&p->iMotEnc->getMotorEncoderAcceleration(off+p->base, accs+l);
        }
        else
            ret=false;
    }
    return ret;
}


bool ControlBoardWrapper::getNumberOfMotorEncoders(int *num) {
    *num=controlledJoints;
    return true;
}

/* IAmplifierControl */

bool ControlBoardWrapper::enableAmp(int j)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    // Use the newer interface if available, otherwise fallback on the old one.
    if(p->iMode2)
        return p->iMode2->setControlMode(off+p->base, VOCAB_CM_IDLE);
    else
        if (p->pos)
        {
            return p->amp->disableAmp(off+p->base);
        }
        return false;
}

bool ControlBoardWrapper::getCurrents(double *vals)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->amp)
        {
            ret=ret&&p->amp->getCurrent(off+p->base, vals+l);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getCurrent(int j, double *val)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->amp)
    {
        return p->amp->getCurrent(off+p->base,val);
    }
    *val=0.0;
    return false;
}

bool ControlBoardWrapper::setMaxCurrent(int j, double v)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

bool ControlBoardWrapper::getAmpStatus(int *st)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (p->amp)
            {

                st[l]=0;
                //getAmpStatus for single joint does not exist!!
                // AMP_STATUS TODO
                //ret=ret&&p->amp->getAmpStatus(off+p->base, st+l);
            }
        else
            ret=false;
    }

    return ret;
}

bool ControlBoardWrapper::getAmpStatus(int j, int *v)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (p->amp)
        {
            return p->amp->getAmpStatus(off+p->base,v);
        }
    *v=0;
    return false;
}

/* IControlLimits */

bool ControlBoardWrapper::setLimits(int j, double min, double max)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->lim2)
    {
        return p->lim2->setLimits(off+p->base,min, max);
    }
    return false;
}

bool ControlBoardWrapper::getLimits(int j, double *min, double *max)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
    {
        *min=0.0;
        *max=0.0;
        return false;
    }

    if (p->lim2)
    {
        return p->lim2->getLimits(off+p->base,min, max);
    }
    *min=0.0;
    *max=0.0;
    return false;
}

bool ControlBoardWrapper::setVelLimits(int j, double min, double max)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (!p->lim2)
    {
        return false;
    }
    return p->lim2->setVelLimits(off+p->base,min, max);
}

bool ControlBoardWrapper::getVelLimits(int j, double *min, double *max)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    *min=0.0;
    *max=0.0;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
    {
        return false;
    }

    if(!p->lim2)
    {
        return false;
    }
    return p->lim2->getVelLimits(off+p->base,min, max);
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

bool ControlBoardWrapper::calibrate(int j, double p)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *s=device.getSubdevice(subIndex);
    if (!s)
        return false;

    if (s->calib)
    {
        return s->calib->calibrate(off+s->base, p);
    }
    return false;
}

bool ControlBoardWrapper::calibrate2(int j, unsigned int ui, double v1, double v2, double v3)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p = device.getSubdevice(subIndex);
    if (p && p->calib2)
    {
        return p->calib2->calibrate2(off+p->base, ui,v1,v2,v3);
    }
    return false;
}

bool ControlBoardWrapper::setCalibrationParameters(int j, const CalibrationParameters& params)
{
    int off = device.lut[j].offset;
    int subIndex = device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p = device.getSubdevice(subIndex);
    if (p && p->calib2)
    {
        return p->calib2->setCalibrationParameters(off + p->base, params);
    }
    return false;
}

bool ControlBoardWrapper::done(int j)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->calib2)
    {
        return p->calib2->done(off+p->base);
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

bool ControlBoardWrapper::getAxisName(int j, yarp::os::ConstString& name)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->info)
    {
        return p->info->getAxisName(off+p->base, name);
    }
    return false;
}

bool ControlBoardWrapper::setTorqueMode()
{
    bool ret=true;
    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

        if (!p)
            return false;

        if(p->iMode2)   // ControlMode2
        {
            ret = ret && p->iMode2->setControlMode(off+p->base, VOCAB_CM_TORQUE);
        }
        else if(p->iMode)
        {
            //calling iControlMode interface
            ret=ret&&p->iMode->setTorqueMode(off+p->base);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getRefTorques(double *refs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            ret=ret&&p->iTorque->getRefTorque(off+p->base, refs+l);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getRefTorque(int j, double *t)
{

    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

    rpcDataMutex.wait();
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
        if(rpcData.subdevices_p[subIndex]->iTorque)   // Position Control 2
        {
            ret= ret && rpcData.subdevices_p[subIndex]->iTorque->setRefTorques(rpcData.subdev_jointsVectorLen[subIndex],
                                                                               rpcData.jointNumbers[subIndex],
                                                                               rpcData.values[subIndex]);
        }
        else   // Classic Position Control
        {
            if(rpcData.subdevices_p[subIndex]->iTorque)
            {
                for(int i = 0; i < rpcData.subdev_jointsVectorLen[subIndex]; i++)
                {
                    ret=ret && rpcData.subdevices_p[subIndex]->iTorque->setRefTorque(rpcData.jointNumbers[subIndex][i],
                                                                                     rpcData.values[subIndex][i]);
                }
            }
            else
            {
                ret=false;
            }
        }
    }
    rpcDataMutex.post();
    return ret;
}


bool ControlBoardWrapper::getBemfParam(int j, double *t)
{

    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->getBemfParam(off+p->base, t);
    }
    return false;
}

bool ControlBoardWrapper::setBemfParam(int j, double t)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->setBemfParam(off+p->base, t);
    }
    return false;
}

bool ControlBoardWrapper::getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->setMotorTorqueParams(off+p->base, params);
    }
    return false;
}

bool ControlBoardWrapper::setTorquePid(int j, const Pid &pid)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->setTorquePid(off+p->base, pid);
    }

    return false;
}

bool ControlBoardWrapper::setImpedance(int j, double stiff, double damp)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            ret=ret&&p->iTorque->getTorque(off+p->base, t+l);
        }
        else
            ret=false;
    }
    return ret;
 }

bool ControlBoardWrapper::getTorqueRange(int j, double *min, double *max)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;
        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            ret=ret&&p->iTorque->getTorqueRange(off+p->base, min+l, max+l);
        }
        else
            ret=false;
    }
    return ret;
 }

bool ControlBoardWrapper::setTorquePids(const Pid *pids)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            ret=ret&&p->iTorque->setTorquePid(off+p->base, pids[l]);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::setTorqueErrorLimit(int j, double limit)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->setTorqueErrorLimit(off+p->base, limit);
    }

    return false;
}

bool ControlBoardWrapper::setTorqueErrorLimits(const double *limits)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            ret=ret&&p->iTorque->setTorqueErrorLimit(off+p->base, limits[l]);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getTorqueError(int j, double *err)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->getTorqueError(off+p->base, err);
    }

    return false;
}

bool ControlBoardWrapper::getTorqueErrors(double *errs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            ret=ret&&p->iTorque->getTorqueError(off+p->base, errs+l);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getTorquePidOutput(int j, double *out)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->getTorquePidOutput(off+p->base, out);
    }

    return false;
}

bool ControlBoardWrapper::getTorquePidOutputs(double *outs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            ret=ret&&p->iTorque->getTorquePidOutput(off+p->base, outs+l);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getTorquePid(int j, Pid *pid)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->getTorquePid(off+p->base, pid);
    }

    return false;
}

bool ControlBoardWrapper::getImpedance(int j, double* stiff, double* damp)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iImpedance)
    {
        return p->iImpedance->getCurrentImpedanceLimit(off+p->base, min_stiff, max_stiff, min_damp, max_damp);
    }

    return false;
}

bool ControlBoardWrapper::getTorquePids(Pid *pids)
{
     bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            ret=ret&&p->iTorque->getTorquePid(off+p->base, pids+l);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getTorqueErrorLimit(int j, double *limit)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->getTorqueErrorLimit(off+p->base, limit);
    }

    return false;
}

bool ControlBoardWrapper::getTorqueErrorLimits(double *limits)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iTorque)
        {
            ret=ret&&p->iTorque->getTorqueErrorLimit(off+p->base, limits+l);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::resetTorquePid(int j)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->resetTorquePid(off+p->base);
    }

    return false;
}

bool ControlBoardWrapper::disableTorquePid(int j)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->disableTorquePid(off+p->base);
    }

    return false;
}

bool ControlBoardWrapper::enableTorquePid(int j)
{
     int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->enableTorquePid(off+p->base);
    }

    return false;
}

bool ControlBoardWrapper::setTorqueOffset(int j, double v)
{
     int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iTorque)
    {
        return p->iTorque->setTorqueOffset(off+p->base,v);
    }

    return false;
}

bool ControlBoardWrapper::setPositionMode(int j)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMode2)
    {
        return p->iMode2->setControlMode(off+p->base, VOCAB_CM_POSITION);
    }
    else
        if (p->iMode)
        {
            return p->iMode->setPositionMode(off+p->base);
        }

    return false;
}

bool ControlBoardWrapper::setTorqueMode(int j)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMode2)
    {
        return p->iMode2->setControlMode(off+p->base, VOCAB_CM_TORQUE);
    }
    else
        if (p->iMode)
        {
            return p->iMode->setTorqueMode(off+p->base);
        }

    return false;
}

bool ControlBoardWrapper::setImpedancePositionMode(int j)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

//        Let´s propagate the legacy version as is until it will be removed
    if (p->iMode)
    {
        return p->iMode->setImpedancePositionMode(off+p->base);
    }

    return false;
}

bool ControlBoardWrapper::setImpedanceVelocityMode(int j)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;
//        Let´s propagate the legacy version as is until it will be removed
    if (p->iMode)
    {
        return p->iMode->setImpedanceVelocityMode(off+p->base);
    }

    return false;
}

bool ControlBoardWrapper::setVelocityMode(int j)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMode2)
    {
        return p->iMode2->setControlMode(off+p->base, VOCAB_CM_VELOCITY);
    }
    else
        if (p->iMode)
        {
            return p->iMode->setVelocityMode(off+p->base);
        }

    return false;
}

bool ControlBoardWrapper::setOpenLoopMode(int j)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMode2)
    {
        return p->iMode2->setControlMode(off+p->base, VOCAB_CM_OPENLOOP);
    }
    else
        if (p->iMode)
        {
            return p->iMode->setOpenLoopMode(off+p->base);
        }

    return false;
}

bool ControlBoardWrapper::getControlMode(int j, int *mode)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
   bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iMode)
        {
            ret=ret&&p->iMode->getControlMode(off+p->base, modes+l);
        }
        else
            ret=false;
    }
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

         yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
         if (!p)
             return false;

         if (p->iMode2)
         {
             ret=ret&&p->iMode2->getControlMode(off+p->base, &modes[l]);
         }
         else
             ret=false;
     }
     return ret;
}

bool ControlBoardWrapper::legacySetControlMode(const int j, const int mode)
{
    bool ret = true;
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    switch(mode)
    {
        case VOCAB_CM_IDLE:
        {

            if(p->amp)
            {
                ret = ret && p->amp->disableAmp(off+p->base);
            }
            if(p->pid)
            {
                ret = ret && p->pid->disablePid(off+p->base);
            }
        }
        break;

        case VOCAB_CM_TORQUE:
        {
            ret = p->iMode->setTorqueMode(off+p->base);
        }
        break;

        case VOCAB_CM_POSITION:
        {
            ret = p->iMode->setPositionMode(off+p->base);
        }
        break;

        case VOCAB_CM_VELOCITY:
        {
            ret = p->iMode->setVelocityMode(off+p->base);
        }
        break;

        case VOCAB_CM_OPENLOOP:
        {
            ret = p->iMode->setOpenLoopMode(off+p->base);
        }
        break;

        case VOCAB_CM_IMPEDANCE_POS:
        {
            ret = p->iMode->setImpedancePositionMode(off+p->base);
        }
        break;

        case VOCAB_CM_IMPEDANCE_VEL:
        {
            ret = p->iMode->setImpedanceVelocityMode(off+p->base);
        }
        break;

        default:
        {
            yError("ControlBoardWrapper received an invalid  setControlMode %s for joint %d\n", yarp::os::Vocab::decode(mode).c_str(), j);
            ret = false;
        }
        break;
    }
    return ret;
}

bool ControlBoardWrapper::setControlMode(const int j, const int mode)
{
    bool ret = true;
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iMode2)
    {
        ret = p->iMode2->setControlMode(off+p->base, mode);
    }
    else
    {
        if (p->iMode)
        {
            legacySetControlMode(j, mode);
        }
    }
    return ret;
}

bool ControlBoardWrapper::setControlModes(const int n_joints, const int *joints, int *modes)
{
    bool ret = true;

    rpcDataMutex.wait();
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
        if(rpcData.subdevices_p[subIndex]->iMode2)   // Position Control 2
        {
            ret= ret && rpcData.subdevices_p[subIndex]->iMode2->setControlModes(rpcData.subdev_jointsVectorLen[subIndex],
                                                                                rpcData.jointNumbers[subIndex],
                                                                                rpcData.modes[subIndex]);
        }
        else   // Classic Position Control
        {
            if(rpcData.subdevices_p[subIndex]->iMode2)
            {
                for(int i = 0; i < rpcData.subdev_jointsVectorLen[subIndex]; i++)
                {
                    ret=ret && rpcData.subdevices_p[subIndex]->iMode2->setControlMode( rpcData.jointNumbers[subIndex][i],
                                                                                       rpcData.modes[subIndex][i]);
                }
            }
            else
            {
                ret=false;
            }
        }
    }
    rpcDataMutex.post();
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
        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

        int wrapped_joints=(p->top - p->base) + 1;
        int *joints = new int[wrapped_joints];

        if(!p)
            return false;

        if(p->iMode2)   // Control Mode interface 2
        {
            // versione comandi su subset di giunti
            for(int j_dev = 0; j_dev < wrapped_joints; j_dev++)
            {
                joints[j_dev] = p->base + j_dev;  // for all joints is equivalent to add offset term
            }

            ret = ret && p->iMode2->setControlModes(wrapped_joints, joints, &modes[j_wrap]);
            j_wrap+=wrapped_joints;
        }
        else
        {
            for(int j_wrap = 0; j_wrap < wrapped_joints; j_wrap++)
            {
                ret = ret && legacySetControlMode(j_wrap, modes[j_wrap]);
            }
        }

        if(joints!=0)
        {
            delete [] joints;
            joints = 0;
        }
    }

    return ret;
}

bool ControlBoardWrapper::setRefOutput(int j, double v)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iOpenLoop)
    {
        return p->iOpenLoop->setRefOutput(off+p->base, v);
    }
    return false;
}

bool ControlBoardWrapper::setRefOutputs(const double *outs) {
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iOpenLoop)
        {
            ret=ret&&p->iOpenLoop->setRefOutput(off+p->base, outs[l]);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::setPosition(int j, double ref)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->posDir)
    {
        return p->posDir->setPosition(off+p->base, ref);
    }

    return false;
}

bool ControlBoardWrapper::setPositionDirectMode()
{
    bool ret=true;
    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);

        if (!p)
            return false;

        if(p->iMode2)
            ret = ret && p->iMode2->setControlMode(off+p->base, VOCAB_CM_POSITION_DIRECT);
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::setPositions(const int n_joints, const int *joints, double *dpos)
{
    bool ret = true;

    rpcDataMutex.wait();
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
    rpcDataMutex.post();
    return ret;
}

bool ControlBoardWrapper::setPositions(const double *refs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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
    timeMutex.wait();
    yarp::os::Stamp ret=time;
    timeMutex.post();
    return ret;
}

//
// IVelocityControl2 Interface
//
bool ControlBoardWrapper::velocityMove(const int n_joints, const int *joints, const double *spds)
{
    bool ret = true;

    rpcDataMutex.wait();
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
        if(rpcData.subdevices_p[subIndex]->vel2)   // Position Control 2
        {
            ret= ret && rpcData.subdevices_p[subIndex]->vel2->velocityMove(rpcData.subdev_jointsVectorLen[subIndex],
                                                                           rpcData.jointNumbers[subIndex],
                                                                           rpcData.values[subIndex]);
        }
        else   // Classic Velocity Control
        {
            if(rpcData.subdevices_p[subIndex]->vel)
            {
                for(int i = 0; i < rpcData.subdev_jointsVectorLen[subIndex]; i++)
                {
                    ret=ret && rpcData.subdevices_p[subIndex]->vel->velocityMove(rpcData.jointNumbers[subIndex][i],
                                                                                 rpcData.values[subIndex][i]);
                }
            }
            else
            {
                ret=false;
            }
        }
    }
    rpcDataMutex.post();
    return ret;
}

bool ControlBoardWrapper::setVelPid(int j, const Pid &pid)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *s=device.getSubdevice(subIndex);
    if (!s)
        return false;

    if (s->vel2)
    {
        return s->vel2->setVelPid(off+s->base, pid);
    }
    return false;
}

bool ControlBoardWrapper::setVelPids(const Pid *pids)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->vel2)
        {
            ret=ret&&p->vel2->setVelPid(off+p->base, pids[l]);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getVelPid(int j, Pid *pid)
{
    //#warning "check for max number of joints!?!?!"
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *s=device.getSubdevice(subIndex);
    if (!s)
        return false;

    if (s->vel2)
    {
        return s->vel2->getVelPid(off+s->base, pid);
    }
    return false;
}

bool ControlBoardWrapper::getVelPids(Pid *pids)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->vel2)
        {
            ret=ret&&p->vel2->getVelPid(off+p->base, pids+l);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::getInteractionMode(int j, yarp::dev::InteractionModeEnum* mode)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *s=device.getSubdevice(subIndex);
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

    rpcDataMutex.wait();
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
        if(rpcData.subdevices_p[subIndex]->iInteract)   // Position Control 2
        {
            ret= ret && rpcData.subdevices_p[subIndex]->iInteract->getInteractionModes(rpcData.subdev_jointsVectorLen[subIndex],
                                                                                       rpcData.jointNumbers[subIndex],
                                                                                       (yarp::dev::InteractionModeEnum*) rpcData.modes[subIndex]);
        }
        else   // Classic Position Control
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
    rpcDataMutex.post();
    return ret;
}

bool ControlBoardWrapper::getInteractionModes(yarp::dev::InteractionModeEnum* modes)
{
    bool ret = true;

    for(int j=0; j<controlledJoints; j++)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iInteract)
        {
            ret=ret && p->iInteract->getInteractionMode(off+p->base, &modes[j]);
        }
        else
            ret=false;
    }
    return ret;
}

bool ControlBoardWrapper::setInteractionMode(int j, yarp::dev::InteractionModeEnum mode)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *s=device.getSubdevice(subIndex);
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

    rpcDataMutex.wait();
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
        if(rpcData.subdevices_p[subIndex]->vel2)   // Position Control 2
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
    rpcDataMutex.post();
    return ret;
}

bool ControlBoardWrapper::setInteractionModes(yarp::dev::InteractionModeEnum* modes)
{
    bool ret = true;

    for(int j=0; j<controlledJoints; j++)
    {
        int off=device.lut[j].offset;
        int subIndex=device.lut[j].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
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

bool ControlBoardWrapper::getRefOutput(int j, double *out)
{
    int off=device.lut[j].offset;
    int subIndex=device.lut[j].deviceEntry;

    yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
    if (!p)
        return false;

    if (p->iOpenLoop)
    {
        return p->iOpenLoop->getRefOutput(off+p->base, out);
    }
    *out=0.0;
    return false;
}

bool ControlBoardWrapper::getRefOutputs(double *outs)
{
    bool ret=true;

    for(int l=0;l<controlledJoints;l++)
    {
        int off=device.lut[l].offset;
        int subIndex=device.lut[l].deviceEntry;

        yarp::dev::impl::SubDevice *p=device.getSubdevice(subIndex);
        if (!p)
            return false;

        if (p->iOpenLoop)
        {
            ret=ret && p->iOpenLoop->getRefOutput(off+p->base, outs+l);
        }
        else
            ret=false;
    }
    return ret;
}
