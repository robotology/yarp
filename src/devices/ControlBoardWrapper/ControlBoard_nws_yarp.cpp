/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoard_nws_yarp.h"

#include "ControlBoardLogComponent.h"
#include "RPCMessagesParser.h"
#include "StreamingMessagesParser.h"

#include <yarp/os/LogStream.h>
#include <yarp/dev/impl/jointData.h>

#include <numeric>


using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using yarp::dev::impl::jointData;

ControlBoard_nws_yarp::ControlBoard_nws_yarp() :
        yarp::os::PeriodicThread(default_period)
{
}

void ControlBoard_nws_yarp::closePorts()
{
    inputRPCPort.interrupt();
    inputRPCPort.removeCallbackLock();
    inputRPCPort.close();

    inputStreamingPort.interrupt();
    inputStreamingPort.close();

    outputPositionStatePort.interrupt();
    outputPositionStatePort.close();

    extendedOutputStatePort.interrupt();
    extendedOutputStatePort.close();
}

bool ControlBoard_nws_yarp::close()
{
    // Ensure that the device is not running
    if (isRunning()) {
        stop();
    }

    closeDevice();
    closePorts();

    return true;
}

bool ControlBoard_nws_yarp::checkPortName(Searchable& params)
{
    // find name as port name (similar both in new and old policy)
    if (!params.check("name")) {
        yCError(CONTROLBOARD) <<
            "*************************************************************************************\n"
            "* controlBoard_nws_yarp missing mandatory parameter 'name' for port name, usage is: *\n"
            "*     name:    full port prefix name with leading '/',  e.g.  /robotName/part/      *\n"
            "*************************************************************************************";
        return false;
    }

    partName = params.find("name").asString();
    if (partName[0] != '/') {
        yCWarning(CONTROLBOARD) <<
            "*************************************************************************************\n"
            "* controlBoard_nws_yarp 'name' parameter for port name does not follow convention,  *\n"
            "* it MUST start with a leading '/' since it is used as the full prefix port name    *\n"
            "*     name:    full port prefix name with leading '/',  e.g.  /robotName/part/      *\n"
            "* A temporary automatic fix will be done for you, but please fix your config file   *\n"
            "*************************************************************************************";
        rootName = "/" + partName;
    } else {
        rootName = partName;
    }

    return true;
}


bool ControlBoard_nws_yarp::open(Searchable& config)
{
    Property prop;
    prop.fromString(config.toString());

    if (!checkPortName(config)) {
        yCError(CONTROLBOARD) << "'portName' was not correctly set, check you r configuration file";
        return false;
    }

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

    rootName = prop.check("rootName", Value("/"), "starting '/' if needed.").asString();
    partName = prop.check("name", Value("controlboard"), "prefix for port names").asString();
    rootName += (partName);
    if (rootName.find("//") != std::string::npos) {
        rootName.replace(rootName.find("//"), 2, "/");
    }

    // Open ports, then attach the readers or callbacks
    if (!inputRPCPort.open((rootName + "/rpc:i"))) {
        yCError(CONTROLBOARD) << "Error opening port " << rootName + "/rpc:i";
        closePorts();
        return false;
    }
    inputRPCPort.setReader(RPC_parser);
    inputRPC_buffer.attach(inputRPCPort);
    RPC_parser.attach(inputRPC_buffer);

    if (!inputStreamingPort.open(rootName + "/command:i")) {
        yCError(CONTROLBOARD) << "Error opening port " << rootName + "/rpc:i";
        closePorts();
        return false;
    }
    inputStreamingPort.setStrict();
    inputStreamingPort.useCallback(streaming_parser);

    if (!outputPositionStatePort.open(rootName + "/state:o")) {
        yCError(CONTROLBOARD) << "Error opening port " << rootName + "/state:o";
        closePorts();
        return false;
    }

    // Extended output state port
    if (!extendedOutputStatePort.open(rootName + "/stateExt:o")) {
        yCError(CONTROLBOARD) << "Error opening port " << rootName + "/state:o";
        closePorts();
        return false;
    }
    extendedOutputState_buffer.attach(extendedOutputStatePort);

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
bool ControlBoard_nws_yarp::openAndAttachSubDevice(Property& prop)
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


bool ControlBoard_nws_yarp::setDevice(yarp::dev::DeviceDriver* driver, bool owned)
{
    // Save the pointer and subDeviceOwned
    subdevice_ptr = driver;
    subdevice_owned = owned;

    // yarp::dev::IPidControl* iPidControl{nullptr};
    subdevice_ptr->view(iPidControl);
    if (!iPidControl) {
        yCWarning(CONTROLBOARD, "Part <%s>: IPidControl interface was not found in subdevice.", partName.c_str());
    }

    // yarp::dev::IPositionControl* iPositionControl{nullptr};
    subdevice_ptr->view(iPositionControl);
    if (!iPositionControl) {
        yCError(CONTROLBOARD, "Part <%s>: IPositionControl interface was not found in subdevice. Quitting", partName.c_str());
        return false;
    }

    // yarp::dev::IPositionDirect* iPositionDirect{nullptr};
    subdevice_ptr->view(iPositionDirect);
    if (!iPositionDirect) {
        yCWarning(CONTROLBOARD, "Part <%s>: IPositionDirect interface was not found in subdevice.", partName.c_str());
    }

    // yarp::dev::IVelocityControl* iVelocityControl{nullptr};
    subdevice_ptr->view(iVelocityControl);
    if (!iVelocityControl) {
        yCError(CONTROLBOARD, "Part <%s>: IVelocityControl interface was not found in subdevice. Quitting", partName.c_str());
        return false;
    }

    // yarp::dev::IEncodersTimed* iEncodersTimed{nullptr};
    subdevice_ptr->view(iEncodersTimed);
    if (!iEncodersTimed) {
        yCError(CONTROLBOARD, "Part <%s>: IEncodersTimed interface was not found in subdevice. Quitting", partName.c_str());
        return false;
    }

    // yarp::dev::IMotor* iMotor{nullptr};
    subdevice_ptr->view(iMotor);
    if (!iMotor) {
        yCWarning(CONTROLBOARD, "Part <%s>: IMotor interface was not found in subdevice.", partName.c_str());
    }

    // yarp::dev::IMotorEncoders* iMotorEncoders{nullptr};
    subdevice_ptr->view(iMotorEncoders);
    if (!iMotorEncoders) {
        yCWarning(CONTROLBOARD, "Part <%s>: IMotorEncoders interface was not found in subdevice.", partName.c_str());
    }

    // yarp::dev::IAmplifierControl* iAmplifierControl{nullptr};
    subdevice_ptr->view(iAmplifierControl);
    if (!iAmplifierControl) {
        yCWarning(CONTROLBOARD, "Part <%s>: IAmplifierControl interface was not found in subdevice.", partName.c_str());
    }

    // yarp::dev::IControlLimits* iControlLimits{nullptr};
    subdevice_ptr->view(iControlLimits);
    if (!iControlLimits) {
        yCWarning(CONTROLBOARD, "Part <%s>: IControlLimits interface was not found in subdevice.", partName.c_str());
    }

    // yarp::dev::IControlCalibration* iControlCalibration{nullptr};
    subdevice_ptr->view(iControlCalibration);
    if (!iControlCalibration) {
        yCWarning(CONTROLBOARD, "Part <%s>: IControlCalibration interface was not found in subdevice.", partName.c_str());
    }

    // yarp::dev::ITorqueControl* iTorqueControl{nullptr};
    subdevice_ptr->view(iTorqueControl);
    if (!iTorqueControl) {
        yCWarning(CONTROLBOARD, "Part <%s>: ITorqueControl interface was not found in subdevice.", partName.c_str());
    }

    // yarp::dev::IImpedanceControl* iImpedanceControl{nullptr};
    subdevice_ptr->view(iImpedanceControl);
    if (!iImpedanceControl) {
        yCWarning(CONTROLBOARD, "Part <%s>: IImpedanceControl interface was not found in subdevice.", partName.c_str());
    }

    // yarp::dev::IControlMode* iControlMode{nullptr};
    subdevice_ptr->view(iControlMode);
    if (!iControlMode) {
        yCWarning(CONTROLBOARD, "Part <%s>: IControlMode interface was not found in subdevice.", partName.c_str());
    }

    // yarp::dev::IAxisInfo* iAxisInfo{nullptr};
    subdevice_ptr->view(iAxisInfo);
    if (!iAxisInfo) {
        yCWarning(CONTROLBOARD, "Part <%s>: IAxisInfo interface was not found in subdevice.", partName.c_str());
    }

    // yarp::dev::IPreciselyTimed* iPreciselyTimed{nullptr};
    subdevice_ptr->view(iPreciselyTimed);
    if (!iPreciselyTimed) {
        yCWarning(CONTROLBOARD, "Part <%s>: IPreciselyTimed interface was not found in subdevice.", partName.c_str());
    }

    // yarp::dev::IInteractionMode* iInteractionMode{nullptr};
    subdevice_ptr->view(iInteractionMode);
    if (!iInteractionMode) {
        yCWarning(CONTROLBOARD, "Part <%s>: IInteractionMode interface was not found in subdevice.", partName.c_str());
    }

    // yarp::dev::IRemoteVariables* iRemoteVariables{nullptr};
    subdevice_ptr->view(iRemoteVariables);
    if (!iRemoteVariables) {
        yCWarning(CONTROLBOARD, "Part <%s>: IRemoteVariables interface was not found in subdevice.", partName.c_str());
    }

    // yarp::dev::IPWMControl* iPWMControl{nullptr};
    subdevice_ptr->view(iPWMControl);
    if (!iPWMControl) {
        yCWarning(CONTROLBOARD, "Part <%s>: IPWMControl interface was not found in subdevice.", partName.c_str());
    }

    // yarp::dev::ICurrentControl* iCurrentControl{nullptr};
    subdevice_ptr->view(iCurrentControl);
    if (!iCurrentControl) {
        yCWarning(CONTROLBOARD, "Part <%s>: ICurrentControl interface was not found in subdevice.", partName.c_str());
    }


    // Get the number of controlled joints
    int tmp_axes;
    if (!iPositionControl->getAxes(&tmp_axes)) {
        yCError(CONTROLBOARD) << "Part <%s>: Failed to get axes number for subdevice " << partName.c_str();
        return false;
    }
    if (tmp_axes <= 0) {
        yCError(CONTROLBOARD, "Part <%s>: attached device has an invalid number of joints (%d)", partName.c_str(), tmp_axes);
        return false;
    }
    subdevice_joints = static_cast<size_t>(tmp_axes);
    times.resize(subdevice_joints);

    // Initialization
    streaming_parser.init(subdevice_ptr);
    streaming_parser.initialize();

    RPC_parser.init(subdevice_ptr);
    RPC_parser.initialize();

    return true;
}

void ControlBoard_nws_yarp::closeDevice()
{
    // Reset callbacks
    streaming_parser.reset();
    RPC_parser.reset();

    // If the subdevice is owned, close and delete the device
    if (subdevice_owned) {
        subdevice_ptr->close();
        delete subdevice_ptr;
    }
    subdevice_ptr = nullptr;
    subdevice_owned = false;
    subdevice_joints = 0;
    subdevice_ready = false;

    times.clear();

    // Clear all interfaces
    iPidControl = nullptr;
    iPositionControl = nullptr;
    iPositionDirect = nullptr;
    iVelocityControl = nullptr;
    iEncodersTimed = nullptr;
    iMotor = nullptr;
    iMotorEncoders = nullptr;
    iAmplifierControl = nullptr;
    iControlLimits = nullptr;
    iControlCalibration = nullptr;
    iTorqueControl = nullptr;
    iImpedanceControl = nullptr;
    iControlMode = nullptr;
    iAxisInfo = nullptr;
    iPreciselyTimed = nullptr;
    iInteractionMode = nullptr;
    iRemoteVariables = nullptr;
    iPWMControl = nullptr;
    iCurrentControl = nullptr;
}

bool ControlBoard_nws_yarp::attach(yarp::dev::PolyDriver* poly)
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

bool ControlBoard_nws_yarp::detach()
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

void ControlBoard_nws_yarp::run()
{
    // check we are not overflowing with input messages
    constexpr int reads_for_warning = 20;
    if (inputStreamingPort.getPendingReads() >= reads_for_warning) {
        yCWarning(CONTROLBOARD) << "Number of streaming input messages to be read is " << inputStreamingPort.getPendingReads() << " and can overflow";
    }

    // Update the port envelope time by averaging all timestamps
    time.update(std::accumulate(times.begin(), times.end(), 0.0) / subdevice_joints);
    yarp::os::Stamp averageTime = time;

    // handle stateExt first
    jointData& data = extendedOutputState_buffer.get();

    data.jointPosition.resize(subdevice_joints);
    data.jointVelocity.resize(subdevice_joints);
    data.jointAcceleration.resize(subdevice_joints);
    data.motorPosition.resize(subdevice_joints);
    data.motorVelocity.resize(subdevice_joints);
    data.motorAcceleration.resize(subdevice_joints);
    data.torque.resize(subdevice_joints);
    data.pwmDutycycle.resize(subdevice_joints);
    data.current.resize(subdevice_joints);
    data.controlMode.resize(subdevice_joints);
    data.interactionMode.resize(subdevice_joints);

    // Get data from HW
    if (iEncodersTimed) {
        data.jointPosition_isValid = iEncodersTimed->getEncodersTimed(data.jointPosition.data(), times.data());
        data.jointVelocity_isValid = iEncodersTimed->getEncoderSpeeds(data.jointVelocity.data());
        data.jointAcceleration_isValid = iEncodersTimed->getEncoderAccelerations(data.jointAcceleration.data());
    } else {
        data.jointPosition_isValid = false;
        data.jointVelocity_isValid = false;
        data.jointAcceleration_isValid = false;
    }

    if (iMotorEncoders) {
        data.motorPosition_isValid = iMotorEncoders->getMotorEncoders(data.motorPosition.data());
        data.motorVelocity_isValid = iMotorEncoders->getMotorEncoderSpeeds(data.motorVelocity.data());
        data.motorAcceleration_isValid = iMotorEncoders->getMotorEncoderAccelerations(data.motorAcceleration.data());
    } else {
        data.motorPosition_isValid = false;
        data.motorVelocity_isValid = false;
        data.motorAcceleration_isValid = false;
    }

    if (iTorqueControl) {
        data.torque_isValid = iTorqueControl->getTorques(data.torque.data());
    } else {
        data.torque_isValid = false;
    }

    if (iPWMControl) {
        data.pwmDutycycle_isValid = iPWMControl->getDutyCycles(data.pwmDutycycle.data());
    } else {
        data.pwmDutycycle_isValid = false;
    }

    if (iCurrentControl) {
        data.current_isValid = iCurrentControl->getCurrents(data.current.data());
    } else if (iAmplifierControl) {
        data.current_isValid = iAmplifierControl->getCurrents(data.current.data());
    } else {
        data.current_isValid = false;
    }

    if (iControlMode) {
        data.controlMode_isValid = iControlMode->getControlModes(data.controlMode.data());
    } else {
        data.controlMode_isValid = false;
    }

    if (iInteractionMode) {
        data.interactionMode_isValid = iInteractionMode->getInteractionModes(reinterpret_cast<yarp::dev::InteractionModeEnum*>(data.interactionMode.data()));
    } else {
        data.interactionMode_isValid = false;
    }

    extendedOutputStatePort.setEnvelope(averageTime);
    extendedOutputState_buffer.write();

    // handle state:o
    yarp::sig::Vector& v = outputPositionStatePort.prepare();
    v.resize(subdevice_joints);
    std::copy(data.jointPosition.begin(), data.jointPosition.end(), v.begin());

    outputPositionStatePort.setEnvelope(averageTime);
    outputPositionStatePort.write();
}
