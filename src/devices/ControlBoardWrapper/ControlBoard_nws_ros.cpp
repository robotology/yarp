/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoard_nws_ros.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <yarp/dev/impl/jointData.h>

#include <yarp/rosmsg/impl/yarpRosHelper.h>

#include "ControlBoardLogComponent.h"

#include <numeric>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

ControlBoard_nws_ros::ControlBoard_nws_ros() :
        yarp::os::PeriodicThread(default_period)
{
}


void ControlBoard_nws_ros::closePorts()
{
    publisherPort.interrupt();
    publisherPort.close();

    delete node;
    node = nullptr;
}

bool ControlBoard_nws_ros::close()
{
    // Ensure that the device is not running
    if (isRunning()) {
        stop();
    }

    closeDevice();
    closePorts();

    return true;
}


bool ControlBoard_nws_ros::open(Searchable& config)
{
    Property prop;
    prop.fromString(config.toString());

    // Check parameter, so if both are present we use the correct one
    if (prop.check("period")) {
        if (!prop.find("period").isFloat64()) {
            yCError(CONTROLBOARD) << "'period' parameter is not a double value";
            return false;
        }
        period = prop.find("period").asFloat64();
        if (period <= 0) {
            yCError(CONTROLBOARD) << "'period' parameter is not valid, read value is" << period;
            return false;
        }
    } else {
        yCDebug(CONTROLBOARD) << "'period' parameter missing, using default thread period = 0.02s";
        period = default_period;
    }

    // Check if we need to create subdevice or if they are
    // passed later on thorugh attachAll()
    if (prop.check("subdevice")) {
        prop.setMonitor(config.getMonitor());
        if (!openAndAttachSubDevice(prop)) {
            yCError(CONTROLBOARD, "Error while opening subdevice");
            return false;
        }
        subdevice_ready = true;
    }

    // check for nodeName parameter
    if (!config.check("node_name")) {
        yCError(CONTROLBOARD) << nodeName << " cannot find node_name parameter";
        return false;
    }
    nodeName = config.find("node_name").asString();
    if(nodeName[0] != '/'){
        yCError(CONTROLBOARD) << "node_name must begin with an initial /";
        return false;
    }

    // check for topicName parameter
    if (!config.check("topic_name")) {
        yCError(CONTROLBOARD) << nodeName << " cannot find topic_name parameter";
        return false;
    }
    topicName = config.find("topic_name").asString();
    if(topicName[0] != '/'){
        yCError(CONTROLBOARD) << "topic_name must begin with an initial /";
        return false;
    }
    topicName.append("/joint_states");
    yCInfo(CONTROLBOARD) << "topicName is " << topicName;
    // call ROS node/topic initialization
    node = new yarp::os::Node(nodeName);
    if (!publisherPort.topic(topicName)) {
        yCError(CONTROLBOARD) << " opening " << topicName << " Topic, check your configuration";
        return false;
    }

    // In case attach is not deferred and the controlboard already owns a valid device
    // we can start the thread. Otherwise this will happen when attachAll is called
    if (subdevice_ready) {
        setPeriod(period);
        if (!start()) {
            yCError(CONTROLBOARD) << "Error starting thread";
            return false;
        }
    }

    return true;
}



// For the simulator, if a subdevice parameter is given to the wrapper, it will
// open it and attach to immediately.
bool ControlBoard_nws_ros::openAndAttachSubDevice(Property& prop)
{
    Property p;
    auto* subDeviceOwned = new PolyDriver;
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

    return setDevice(subDeviceOwned, true);
}


bool ControlBoard_nws_ros::setDevice(yarp::dev::DeviceDriver* driver, bool owned)
{
    yCAssert(CONTROLBOARD, driver);

    // Save the pointer and subDeviceOwned
    subdevice_ptr = driver;
    subdevice_owned = owned;

    subdevice_ptr->view(iPositionControl);
    if (!iPositionControl) {
        yCError(CONTROLBOARD, "<%s - %s>: IPositionControl interface was not found in subdevice. Quitting", nodeName.c_str(), topicName.c_str());
        return false;
    }

    subdevice_ptr->view(iEncodersTimed);
    if (!iEncodersTimed) {
        yCError(CONTROLBOARD, "<%s - %s>: IEncodersTimed interface was not found in subdevice. Quitting", nodeName.c_str(), topicName.c_str());
        return false;
    }

    subdevice_ptr->view(iTorqueControl);
    if (!iTorqueControl) {
        yCWarning(CONTROLBOARD, "<%s - %s>: ITorqueControl interface was not found in subdevice.", nodeName.c_str(), topicName.c_str());
    }

    subdevice_ptr->view(iAxisInfo);
    if (!iAxisInfo) {
        yCError(CONTROLBOARD, "<%s - %s>: IAxisInfo interface was not found in subdevice. Quitting", nodeName.c_str(), topicName.c_str());
        return false;
    }

    // Get the number of controlled joints
    int tmp_axes;
    if (!iPositionControl->getAxes(&tmp_axes)) {
        yCError(CONTROLBOARD, "<%s - %s>: Failed to get axes number for subdevice ", nodeName.c_str(), topicName.c_str());
        return false;
    }
    if (tmp_axes <= 0) {
        yCError(CONTROLBOARD, "<%s - %s>: attached device has an invalid number of joints (%d)", nodeName.c_str(), topicName.c_str(), tmp_axes);
        return false;
    }
    subdevice_joints = static_cast<size_t>(tmp_axes);
    times.resize(subdevice_joints);
    ros_struct.name.resize(subdevice_joints);
    ros_struct.position.resize(subdevice_joints);
    ros_struct.velocity.resize(subdevice_joints);
    ros_struct.effort.resize(subdevice_joints);

    if (!updateAxisName()) {
        return false;
    }

    return true;
}


void ControlBoard_nws_ros::closeDevice()
{
    // If the subdevice is owned, close and delete the device
    if (subdevice_owned) {
        yCAssert(CONTROLBOARD, subdevice_ptr);
        subdevice_ptr->close();
        delete subdevice_ptr;
    }
    subdevice_ptr = nullptr;
    subdevice_owned = false;
    subdevice_joints = 0;
    subdevice_ready = false;

    times.clear();

    // Clear all interfaces
    iPositionControl = nullptr;
    iEncodersTimed = nullptr;
    iTorqueControl = nullptr;
    iAxisInfo = nullptr;
}

bool ControlBoard_nws_ros::attach(yarp::dev::PolyDriver* poly)
{
    // Check if we already instantiated a subdevice previously
    if (subdevice_ready) {
        return false;
    }

    if (!setDevice(poly, false)) {
        return false;
    }

    setPeriod(period);
    if (!start()) {
        yCError(CONTROLBOARD) << "Error starting thread";
        return false;
    }

    return true;
}

bool ControlBoard_nws_ros::detach()
{
    //check if we already instantiated a subdevice previously
    if (subdevice_owned) {
        return false;
    }

    // Ensure that the device is not running
    if (isRunning()) {
        stop();
    }

    closeDevice();

    return true;
}


bool ControlBoard_nws_ros::updateAxisName()
{
    // IMPORTANT!! This function has to be called BEFORE the thread starts,
    // the name has to be correct right from the first message!!

    yCAssert(CONTROLBOARD, iAxisInfo);

    std::vector<std::string> tmpVect;
    for (size_t i = 0; i < subdevice_joints; i++) {
        std::string tmp;
        bool ret = iAxisInfo->getAxisName(i, tmp);
        if (!ret) {
            yCError(CONTROLBOARD, "Joint name for axis %zu not found!", i);
            return false;
        }
        tmpVect.emplace_back(tmp);
    }

    yCAssert(CONTROLBOARD, tmpVect.size() == subdevice_joints);

    jointNames = tmpVect;

    return true;
}


void ControlBoard_nws_ros::run()
{
    yCAssert(CONTROLBOARD, iEncodersTimed);
    yCAssert(CONTROLBOARD, iAxisInfo);

    bool positionsOk = iEncodersTimed->getEncodersTimed(ros_struct.position.data(), times.data());
    YARP_UNUSED(positionsOk);

    bool speedsOk = iEncodersTimed->getEncoderSpeeds(ros_struct.velocity.data());
    YARP_UNUSED(speedsOk);

    if (iTorqueControl) {
        bool torqueOk = iTorqueControl->getTorques(ros_struct.effort.data());
        YARP_UNUSED(torqueOk);
    }

    // Update the port envelope time by averaging all timestamps
    time.update(std::accumulate(times.begin(), times.end(), 0.0) / subdevice_joints);
    yarp::os::Stamp averageTime = time;

    // Data from HW have been gathered few lines before
    JointTypeEnum jType;
    for (size_t i = 0; i < subdevice_joints; i++) {
        iAxisInfo->getJointType(i, jType);
        if (jType == VOCAB_JOINTTYPE_REVOLUTE) {
            ros_struct.position[i] = convertDegreesToRadians(ros_struct.position[i]);
            ros_struct.velocity[i] = convertDegreesToRadians(ros_struct.velocity[i]);
        }
    }

    ros_struct.name = jointNames;

    ros_struct.header.seq = counter++;
    ros_struct.header.stamp = averageTime.getTime();

    publisherPort.write(ros_struct);
}
