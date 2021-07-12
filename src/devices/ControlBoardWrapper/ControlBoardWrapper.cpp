/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapper.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <yarp/dev/impl/jointData.h>

#include "ControlBoardLogComponent.h"
#include "RPCMessagesParser.h"
#include "StreamingMessagesParser.h"
#include <algorithm>
#include <cstring> // for memset function
#include <iostream>
#include <numeric>
#include <sstream>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::impl;
using namespace yarp::sig;
using namespace std;


ControlBoardWrapper::ControlBoardWrapper() :
        yarp::os::PeriodicThread(default_period)
{
    streaming_parser.init(this);
    RPC_parser.init(this);
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

    if (yarp::os::PeriodicThread::isRunning()) {
        yarp::os::PeriodicThread::stop();
    }

    if (useROS != ROS_only) {
        cleanup_yarpPorts();
    }

    if (rosNode != nullptr) {
        delete rosNode;
        rosNode = nullptr;
    }

    //if we own a deviced we have to close and delete it
    if (ownDevices) {
        // we should have created a new devices which we need to delete
        if (subDeviceOwned != nullptr) {
            subDeviceOwned->close();
            delete subDeviceOwned;
            subDeviceOwned = nullptr;
        }
    } else {
        detachAll();
    }
    return true;
}

bool ControlBoardWrapper::checkPortName(Searchable& params)
{
    /* see if rootName is present in the config file, this param is not used from long time, so it'll be
     * marked as deprecated.
     */
    if (params.check("rootName")) {
        yCWarning(CONTROLBOARD) <<
            "************************************************************************************\n"
            "* controlboardwrapper2 is using the deprecated parameter 'rootName' for port name, *\n"
            "* It has to be removed and substituted with:                                       *\n"
            "*     name:    full port prefix name with leading '/',  e.g.  /robotName/part/     *\n"
            "************************************************************************************";
        rootName = params.find("rootName").asString();
    }

    // find name as port name (similar both in new and old policy
    if (!params.check("name")) {
        yCError(CONTROLBOARD) <<
            "************************************************************************************\n"
            "* controlboardwrapper2 missing mandatory parameter 'name' for port name, usage is: *\n"
            "*     name:    full port prefix name with leading '/',  e.g.  /robotName/part/     *\n"
            "************************************************************************************";
        return false;
    }

    partName = params.find("name").asString();
    if (partName[0] != '/') {
        yCWarning(CONTROLBOARD) <<
            "************************************************************************************\n"
            "* controlboardwrapper2 'name' parameter for port name does not follow convention,  *\n"
            "* it MUST start with a leading '/' since it is used as the full prefix port name   *\n"
            "*     name:    full port prefix name with leading '/',  e.g.  /robotName/part/     *\n"
            "* A temporary automatic fix will be done for you, but please fix your config file  *\n"
            "************************************************************************************";
        rootName = "/" + partName;
    } else {
        rootName = partName;
    }

    return true;
}

bool ControlBoardWrapper::checkROSParams(Searchable& config)
{
    // check for ROS parameter group
    if (!config.check("ROS")) {
        useROS = ROS_disabled;
        return true;
    }

    yCInfo(CONTROLBOARD) << "ROS group was FOUND in config file.";

    Bottle& rosGroup = config.findGroup("ROS");
    if (rosGroup.isNull()) {
        yCError(CONTROLBOARD) << partName << "ROS group params is not a valid group or empty";
        useROS = ROS_config_error;
        return false;
    }

    // check for useROS parameter
    if (!rosGroup.check("useROS")) {
        yCError(CONTROLBOARD) << partName << " cannot find useROS parameter, mandatory when using ROS message. \n \
                    Allowed values are true, false, ROS_only";
        useROS = ROS_config_error;
        return false;
    }
    std::string ros_use_type = rosGroup.find("useROS").asString();
    if (ros_use_type == "false") {
        yCInfo(CONTROLBOARD) << partName << "useROS topic if set to 'false'";
        useROS = ROS_disabled;
        return true;
    }

    if (ros_use_type == "true") {
        yCInfo(CONTROLBOARD) << partName << "useROS topic if set to 'true'";
        useROS = ROS_enabled;
    } else if (ros_use_type == "only") {
        yCInfo(CONTROLBOARD) << partName << "useROS topic if set to 'only";
        useROS = ROS_only;
    } else {
        yCInfo(CONTROLBOARD) << partName << "useROS parameter is seet to unvalid value ('" << ros_use_type << "'), supported values are 'true', 'false', 'only'";
        useROS = ROS_config_error;
        return false;
    }

    // check for ROS_nodeName parameter
    if (!rosGroup.check("ROS_nodeName")) {
        yCError(CONTROLBOARD) << partName << " cannot find ROS_nodeName parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    rosNodeName = rosGroup.find("ROS_nodeName").asString();
    if(rosNodeName[0] != '/'){
        yCError(CONTROLBOARD) << "ros node name must begin with an initial /";
        return false;
    }
    yCInfo(CONTROLBOARD) << partName << "rosNodeName is " << rosNodeName;

    // check for ROS_topicName parameter
    if (!rosGroup.check("ROS_topicName")) {
        yCError(CONTROLBOARD) << partName << " cannot find rosTopicName parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    rosTopicName = rosGroup.find("ROS_topicName").asString();
    yCInfo(CONTROLBOARD) << partName << "rosTopicName is " << rosTopicName;

    // check for rosNodeName parameter
    // UPDATE: joint names are got from MotionControl subdevice now.
    // An error should be thrown later on in case we fail getting names from device
    if (!rosGroup.check("jointNames")) {
        yCInfo(CONTROLBOARD) << partName << "ROS topic has been required, jointNames will be got from motionControl subdevice.";
    } else // if names are there, store them. They will be used for back compatibility if old policy is used.
    {
        Bottle nameList = rosGroup.findGroup("jointNames").tail();
        if (nameList.isNull()) {
            yCError(CONTROLBOARD) << partName << " jointNames not found";
            useROS = ROS_config_error;
            return false;
        }

        if (nameList.size() != static_cast<size_t>(controlledJoints)) {
            yCError(CONTROLBOARD) << partName << " jointNames incorrect number of entries. \n jointNames is " << nameList.toString() << "while expected length is " << controlledJoints;
            useROS = ROS_config_error;
            return false;
        }

        jointNames.clear();
        for (size_t i = 0; i < controlledJoints; i++) {
            jointNames.push_back(nameList.get(i).toString());
        }
    }
    return true;
}

bool ControlBoardWrapper::initialize_ROS()
{
    bool success = false;
    switch (useROS) {
    case ROS_enabled:
    case ROS_only: {
        rosNode = new yarp::os::Node(rosNodeName); // add a ROS node

        if (rosNode == nullptr) {
            yCError(CONTROLBOARD) << " opening " << rosNodeName << " Node, check your yarp-ROS network configuration";
            success = false;
            break;
        }

        if (!rosPublisherPort.topic(rosTopicName)) {
            yCError(CONTROLBOARD) << " opening " << rosTopicName << " Topic, check your yarp-ROS network configuration";
            success = false;
            break;
        }
        success = true;
    } break;

    case ROS_disabled: {
        yCInfo(CONTROLBOARD) << partName << ": no ROS initialization required";
        success = true;
    } break;

    case ROS_config_error: {
        yCError(CONTROLBOARD) << partName << " ROS parameter are not correct, check your configuration file";
        success = false;
    } break;

    default:
    {
        yCError(CONTROLBOARD) << partName << " something went wrong with ROS configuration, we should never be here!!!";
        success = false;
    } break;
    }
    return success;
}

bool ControlBoardWrapper::initialize_YARP(yarp::os::Searchable& prop)
{
    bool success = false;

    switch (useROS) {
    case ROS_only: {
        yCInfo(CONTROLBOARD) << partName << " No YARP initialization required";
        success = true;
    } break;

    default:
    {
        yCInfo(CONTROLBOARD) << partName << " initting YARP initialization";
        // initialize callback
        if (!streaming_parser.initialize()) {
            yCError(CONTROLBOARD) << "Error could not initialize callback object";
            success = false;
            break;
        }

        rootName = prop.check("rootName", Value("/"), "starting '/' if needed.").asString();
        partName = prop.check("name", Value("controlboard"), "prefix for port names").asString();
        rootName += (partName);
        if (rootName.find("//") != std::string::npos) {
            rootName.replace(rootName.find("//"), 2, "/");
        }

        ///// We now open ports, then attach the readers or callbacks
        if (!inputRPCPort.open((rootName + "/rpc:i"))) {
            yCError(CONTROLBOARD) << "Error opening port " << rootName + "/rpc:i";
            success = false;
            break;
        }
        inputRPCPort.setReader(RPC_parser);
        inputRPC_buffer.attach(inputRPCPort);
        RPC_parser.attach(inputRPC_buffer);

        if (!inputStreamingPort.open(rootName + "/command:i")) {
            yCError(CONTROLBOARD) << "Error opening port " << rootName + "/rpc:i";
            success = false;
            break;
        }

        // attach callback.
        inputStreamingPort.setStrict();
        inputStreamingPort.useCallback(streaming_parser);

        if (!outputPositionStatePort.open(rootName + "/state:o")) {
            yCError(CONTROLBOARD) << "Error opening port " << rootName + "/state:o";
            success = false;
            break;
        }

        // new extended output state port
        if (!extendedOutputStatePort.open(rootName + "/stateExt:o")) {
            yCError(CONTROLBOARD) << "Error opening port " << rootName + "/state:o";
            success = false;
            break;
        }
        extendedOutputState_buffer.attach(extendedOutputStatePort);
        success = true;
    } break;
    } // end switch

    // cleanup if something went wrong
    if (!success) {
        cleanup_yarpPorts();
    }
    return success;
}


bool ControlBoardWrapper::open(Searchable& config)
{
    yCWarning(CONTROLBOARD) << "The 'controlboardwrapper2' device is deprecated in favour of 'controlboardremapper' + 'controlBoard_nws_yarp'.";
    yCWarning(CONTROLBOARD) << "The old device is no longer supported, and it will be deprecated in YARP 3.6 and removed in YARP 4.";
    yCWarning(CONTROLBOARD) << "Please update your scripts.";

    Property prop;
    prop.fromString(config.toString());

    if (prop.check("verbose", "Deprecated flag. Use log components instead")) {
        yCWarning(CONTROLBOARD) << "'verbose' flag is deprecated. Use log components instead";
    }

    if (!checkPortName(config)) {
        yCError(CONTROLBOARD) << "'portName' was not correctly set, check you r configuration file";
        return false;
    }

    // check FIRST for deprecated parameter
    if (prop.check("threadrate")) {
        yCError(CONTROLBOARD) << "Using removed parameter 'threadrate', use 'period' instead";
        return false;
    }

    // NOW, check for correct parameter, so if both are present we use the correct one
    if (prop.check("period")) {
        if (!prop.find("period").isInt32()) {
            yCError(CONTROLBOARD) << "'period' parameter is not an integer value";
            return false;
        }
        period = prop.find("period").asInt32() / 1000.0;
        if (period <= 0) {
            yCError(CONTROLBOARD) << "'period' parameter is not valid, read value is" << period;
            return false;
        }
    } else {
        yCDebug(CONTROLBOARD) << "'period' parameter missing, using default thread period = 20ms";
        period = default_period;
    }

    // check if we need to create subdevice or if they are
    // passed later on thorugh attachAll()
    if (prop.check("subdevice")) {
        ownDevices = true;
        prop.setMonitor(config.getMonitor());
        if (!openAndAttachSubDevice(prop)) {
            yCError(CONTROLBOARD, "Error while opening subdevice");
            return false;
        }
    } else {
        ownDevices = false;
        if (!openDeferredAttach(prop)) {
            return false;
        }
    }

    // using controlledJoints here will allocate more memory than required, but not so much.
    rpcData.resize(device.subdevices.size(), controlledJoints, &device);

    /* This must be after the openAndAttachSubDevice() or openDeferredAttach() in order to have the correct number of controlledJoints,
        but before the initialize_ROS and initialize_YARP */
    if (!checkROSParams(config)) {
        yCError(CONTROLBOARD) << partName << " ROS parameter are not correct, check your configuration file";
        return false;
    }

    // call ROS node/topic initialization, if needed
    if (!initialize_ROS()) {
        return false;
    }

    // call YARP port initialization, if needed
    if (!initialize_YARP(prop)) {
        yCError(CONTROLBOARD) << partName << "Something wrong when initting yarp ports";
        return false;
    }

    times.resize(controlledJoints);
    ros_struct.name.resize(controlledJoints);
    ros_struct.position.resize(controlledJoints);
    ros_struct.velocity.resize(controlledJoints);
    ros_struct.effort.resize(controlledJoints);

    // In case attach is not deferred and the controlboard already owns a valid device
    // we can start the thread. Otherwise this will happen when attachAll is called
    if (ownDevices) {
        PeriodicThread::setPeriod(period);
        if (!PeriodicThread::start()) {
            return false;
        }
    }
    return true;
}


// Default usage
// Open the wrapper only, the attach method needs to be called before using it
bool ControlBoardWrapper::openDeferredAttach(Property& prop)
{
    if (!prop.check("networks", "list of networks merged by this wrapper")) {
        yCError(CONTROLBOARD) << "List of networks to attach to was not found.";
        return false;
    }

    Bottle* nets = prop.find("networks").asList();
    if (nets == nullptr) {
        yCError(CONTROLBOARD) << "Error parsing parameters: \"networks\" should be followed by a list";
        return false;
    }

    if (!prop.check("joints", "number of joints of the part")) {
        return false;
    }

    controlledJoints = prop.find("joints").asInt32();

    size_t nsubdevices = nets->size();
    device.lut.resize(controlledJoints);
    device.subdevices.resize(nsubdevices);

    // configure the devices
    size_t totalJ = 0;
    for (size_t k = 0; k < nets->size(); k++) {
        Bottle parameters;
        size_t wBase;
        size_t wTop;
        size_t base;
        size_t top;

        parameters = prop.findGroup(nets->get(k).asString());

        if (parameters.size() == 2) {
            Bottle* bot = parameters.get(1).asList();
            Bottle tmpBot;
            if (bot == nullptr) {
                // probably data are not passed in the correct way, try to read them as a string.
                std::string bString(parameters.get(1).asString());
                tmpBot.fromString(bString);

                if (tmpBot.size() != 4) {
                    yCError(CONTROLBOARD) << "Error: check network parameters in part description"
                                                 << "--> I was expecting " << nets->get(k).asString() << " followed by a list of four integers in parenthesis"
                                                 << "Got: " << parameters.toString();
                    return false;
                }

                bot = &tmpBot;
            }

            // If I came here, bot is correct
            wBase = static_cast<size_t>(bot->get(0).asInt32());
            wTop = static_cast<size_t>(bot->get(1).asInt32());
            base = static_cast<size_t>(bot->get(2).asInt32());
            top = static_cast<size_t>(bot->get(3).asInt32());
        } else if (parameters.size() == 5) {
            yCError(CONTROLBOARD) << "Parameter networks use deprecated syntax";
            wBase = static_cast<size_t>(parameters.get(1).asInt32());
            wTop = static_cast<size_t>(parameters.get(2).asInt32());
            base = static_cast<size_t>(parameters.get(3).asInt32());
            top = static_cast<size_t>(parameters.get(4).asInt32());
        } else {
            yCError(CONTROLBOARD) << "Error: check network parameters in part description"
                                         << "--> I was expecting " << nets->get(k).asString() << " followed by a list of four integers in parenthesis"
                                         << "Got: " << parameters.toString();
            return false;
        }

        SubDevice* tmpDevice = device.getSubdevice(k);
        if (!tmpDevice) {
            yCError(CONTROLBOARD) << "Get of subdevice returned null";
            return false;
        }

        size_t axes = top - base + 1;
        if (!tmpDevice->configure(wBase, wTop, base, top, axes, nets->get(k).asString(), getId())) {
            yCError(CONTROLBOARD) << "Configure of subdevice ret false";
            return false;
        }

        // Check input values are in range
        if ((wBase == static_cast<size_t>(-1)) || (wBase >= controlledJoints)) {
            yCError(CONTROLBOARD) << "Input configuration for device " << partName << "has a wrong attach map."
                                         << "First index " << wBase << "must be inside range from 0 to 'joints' (" << controlledJoints << ")";
            return false;
        }

        if ((wTop == static_cast<size_t>(-1)) || (wTop >= controlledJoints)) {
            yCError(CONTROLBOARD) << "Input configuration for device " << partName << "has a wrong attach map."
                                         << "Second index " << wTop << "must be inside range from 0 to 'joints' (" << controlledJoints << ")";
            return false;
        }

        if (wBase > wTop) {
            yCError(CONTROLBOARD) << "Input configuration for device " << partName << "has a wrong attach map."
                                         << "First index " << wBase << "must be lower than  second index " << wTop;
            return false;
        }

        for (size_t j = wBase, jInDev = base; j <= wTop; j++, jInDev++) {
            device.lut[j].deviceEntry = k;
            device.lut[j].offset = static_cast<int>(j - wBase);
            device.lut[j].jointIndexInDev = jInDev;
        }

        totalJ += axes;
    }

    if (totalJ != controlledJoints) {
        yCError(CONTROLBOARD) << "Error total number of mapped joints (" << totalJ << ") does not correspond to part joints (" << controlledJoints << ")";
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
    p.put("device", subdevice); // subdevice was already checked before

    // if errors occurred during open, quit here.
    yCDebug(CONTROLBOARD, "opening subdevice");
    subDeviceOwned->open(p);

    if (!subDeviceOwned->isValid()) {
        yCError(CONTROLBOARD, "opening subdevice... FAILED");
        return false;
    }

    yarp::dev::IEncoders* iencs = nullptr;

    subDeviceOwned->view(iencs);

    if (iencs == nullptr) {
        yCError(CONTROLBOARD, "Opening IEncoders interface of subdevice... FAILED");
        return false;
    }

    int tmp_axes;
    bool getAx = iencs->getAxes(&tmp_axes);

    if (!getAx) {
        yCError(CONTROLBOARD, "Calling getAxes of subdevice... FAILED");
        return false;
    }
    controlledJoints = static_cast<size_t>(tmp_axes);

    yCDebug(CONTROLBOARD, "Joints parameter is %zu", controlledJoints);


    device.lut.resize(controlledJoints);
    device.subdevices.resize(1);

    // configure the device
    size_t base = 0;
    size_t top = controlledJoints - 1;

    size_t wbase = base;
    size_t wtop = top;
    SubDevice* tmpDevice = device.getSubdevice(0);

    std::string subDevName((partName + "_" + subdevice));
    if (!tmpDevice->configure(wbase, wtop, base, top, controlledJoints, subDevName, getId())) {
        yCError(CONTROLBOARD) << "Configure of subdevice ret false";
        return false;
    }

    for (size_t j = 0; j < controlledJoints; j++) {
        device.lut[j].deviceEntry = 0;
        device.lut[j].offset = j;
    }


    if (!device.subdevices[0].attach(subDeviceOwned, subDevName)) {
        return false;
    }

    // initialization.
    RPC_parser.initialize();
    updateAxisName();
    calculateMaxNumOfJointsInDevices();
    return true;
}

void ControlBoardWrapper::calculateMaxNumOfJointsInDevices()
{
    device.maxNumOfJointsInDevices = 0;

    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (p->totalAxis > device.maxNumOfJointsInDevices) {
            device.maxNumOfJointsInDevices = p->totalAxis;
        }
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

    // no need to update this variable if we are not using ROS. YARP RPC will always call the sudevice.
    if (useROS == ROS_disabled) {
        return true;
    }

    std::string tmp;
    // I need a temporary vector because if I'm wrapping more than one subdevice, and one of them
    // does not have the axesName, then I'd stick with the old names from wrpper config file, if any.
    vector<string> tmpVect;
    bool ret = true;

    tmpVect.clear();
    for (size_t i = 0; i < controlledJoints; i++) {
        if ((ret = getAxisName(i, tmp) && ret)) {
            std::string tmp2(tmp);
            tmpVect.push_back(tmp2);
        }
    }

    if (ret) {
        if (!jointNames.empty()) {
            yCWarning(CONTROLBOARD) << "Found 2 instance of jointNames parameter: one in the wrapper [ROS] group and another one in the subdevice, the latter one will be used.";
            std::string fullNames;
            for (size_t i = 0; i < controlledJoints; i++) {
                fullNames.append(tmpVect[i]);
            }
        }

        jointNames.clear();
        jointNames = tmpVect;
    } else {
        if (jointNames.empty()) {
            yCError(CONTROLBOARD) << "Joint names were not found! they are mandatory when using ROS topic";
            return false;
        }

        yCWarning(CONTROLBOARD) << "\n" <<
            "************************************************************************************************** \n" <<
            "* Joint names for ROS topic were found in the [ROS] group in the wrapper config file for\n" <<
            "* '" << partName << "' device.\n" <<
            "* They should be in the MotionControl device(s) instead. Please update the config files.\n" <<
            "**************************************************************************************************";
    }
    return true;
}


bool ControlBoardWrapper::attachAll(const PolyDriverList& polylist)
{
    //check if we already instantiated a subdevice previously
    if (ownDevices) {
        return false;
    }

    for (int p = 0; p < polylist.size(); p++) {
        // look if we have to attach to a calibrator
        std::string tmpKey = polylist[p]->key;
        if (tmpKey == "Calibrator" || tmpKey == "calibrator") {
            // Set the IRemoteCalibrator interface, the wrapper must point to the calibrator device
            yarp::dev::IRemoteCalibrator* calibrator;
            polylist[p]->poly->view(calibrator);

            IRemoteCalibrator::setCalibratorDevice(calibrator);
            continue;
        }

        // find appropriate entry in list of subdevices and attach
        size_t k = 0;
        for (k = 0; k < device.subdevices.size(); k++) {
            if (device.subdevices[k].id == tmpKey) {
                if (!device.subdevices[k].attach(polylist[p]->poly, tmpKey)) {
                    yCError(CONTROLBOARD, "Attach to subdevice %s failed", polylist[p]->key.c_str());
                    return false;
                }
            }
        }
    }

    //check if all devices are attached to the driver
    bool ready = true;
    for (auto& subdevice : device.subdevices) {
        if (!subdevice.isAttached()) {
            yCError(CONTROLBOARD, "Device %s was not found in the list passed to attachAll", subdevice.id.c_str());
            ready = false;
        }
    }

    if (!ready) {
        yCError(CONTROLBOARD, "AttachAll failed, some subdevice was not found or its attach failed");
        stringstream ss;
        for (int p = 0; p < polylist.size(); p++) {
            ss << polylist[p]->key.c_str() << " ";
        }
        yCError(CONTROLBOARD, "List of devices keys passed to attachAll: %s", ss.str().c_str());
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
    if (ownDevices) {
        return false;
    }

    if (yarp::os::PeriodicThread::isRunning()) {
        yarp::os::PeriodicThread::stop();
    }

    int devices = device.subdevices.size();

    for (int k = 0; k < devices; k++) {
        SubDevice* sub = device.getSubdevice(k);
        if (sub) {
            sub->detach();
        }
    }

    IRemoteCalibrator::releaseCalibratorDevice();
    return true;
}

void ControlBoardWrapper::run()
{
    // check we are not overflowing with input messages
    if (inputStreamingPort.getPendingReads() >= 20) {
        yCWarning(CONTROLBOARD) << "Number of streaming input messages to be read is " << inputStreamingPort.getPendingReads() << " and can overflow";
    }

    // Small optimization: Avoid to call getEncoders twice, one for YARP port
    // and again for ROS topic.
    //
    // Calling getStuff here on ros_struct because it is a class member, hence
    // always available. In the other side, to have the yarp struct to write into
    // it will be rewuired to call port.prepare, that it is something I should
    // not do if the wrapper is in ROS_only configuration.

    bool positionsOk = getEncodersTimed(ros_struct.position.data(), times.data());
    bool speedsOk = getEncoderSpeeds(ros_struct.velocity.data());
    bool torqueOk = getTorques(ros_struct.effort.data());

    // Update the port envelope time by averaging all timestamps
    timeMutex.lock();
    time.update(std::accumulate(times.begin(), times.end(), 0.0) / controlledJoints);
    yarp::os::Stamp averageTime = time;
    timeMutex.unlock();

    if (useROS != ROS_only) {
        // handle stateExt first
        jointData& yarp_struct = extendedOutputState_buffer.get();

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
        yarp_struct.jointPosition_isValid = positionsOk;
        std::copy(ros_struct.position.begin(), ros_struct.position.end(), yarp_struct.jointPosition.begin());

        yarp_struct.jointVelocity_isValid = speedsOk;
        std::copy(ros_struct.velocity.begin(), ros_struct.velocity.end(), yarp_struct.jointVelocity.begin());

        yarp_struct.torque_isValid = torqueOk;
        std::copy(ros_struct.effort.begin(), ros_struct.effort.end(), yarp_struct.torque.begin());

        // Get remaining data from HW
        yarp_struct.jointAcceleration_isValid = getEncoderAccelerations(yarp_struct.jointAcceleration.data());
        yarp_struct.motorPosition_isValid = getMotorEncoders(yarp_struct.motorPosition.data());
        yarp_struct.motorVelocity_isValid = getMotorEncoderSpeeds(yarp_struct.motorVelocity.data());
        yarp_struct.motorAcceleration_isValid = getMotorEncoderAccelerations(yarp_struct.motorAcceleration.data());
        yarp_struct.pwmDutycycle_isValid = getDutyCycles(yarp_struct.pwmDutycycle.data());
        yarp_struct.current_isValid = ControlBoardWrapperCommon::getCurrents(yarp_struct.current.data());
        yarp_struct.controlMode_isValid = getControlModes(yarp_struct.controlMode.data());
        yarp_struct.interactionMode_isValid = getInteractionModes(reinterpret_cast<yarp::dev::InteractionModeEnum*>(yarp_struct.interactionMode.data()));

        extendedOutputStatePort.setEnvelope(averageTime);
        extendedOutputState_buffer.write();

        // handle state:o
        yarp::sig::Vector& v = outputPositionStatePort.prepare();
        v.resize(controlledJoints);
        std::copy(yarp_struct.jointPosition.begin(), yarp_struct.jointPosition.end(), v.begin());

        outputPositionStatePort.setEnvelope(averageTime);
        outputPositionStatePort.write();
    }

    if (useROS != ROS_disabled) {
        // Data from HW have been gathered few lines before
        JointTypeEnum jType;
        for (size_t i = 0; i < controlledJoints; i++) {
            getJointType(i, jType);
            if (jType == VOCAB_JOINTTYPE_REVOLUTE) {
                ros_struct.position[i] = convertDegreesToRadians(ros_struct.position[i]);
                ros_struct.velocity[i] = convertDegreesToRadians(ros_struct.velocity[i]);
            }
        }

        ros_struct.name = jointNames;

        ros_struct.header.seq = rosMsgCounter++;
        ros_struct.header.stamp = averageTime.getTime();

        rosPublisherPort.write(ros_struct);
    }
}
