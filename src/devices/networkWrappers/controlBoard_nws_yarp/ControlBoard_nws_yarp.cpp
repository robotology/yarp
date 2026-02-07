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
#include <cmath>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using yarp::dev::impl::jointData;

const double DEFAULT_PERIOD = 0.02;

ControlBoard_nws_yarp::ControlBoard_nws_yarp() :
        yarp::os::PeriodicThread(DEFAULT_PERIOD)
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

bool ControlBoard_nws_yarp::open(Searchable& config)
{
    if (!parseParams(config)) { return false; }

    // open rpc port
    std::string server_rpc_portname = m_name + "/nws/rpc";
    if (!m_rpcPort.open(server_rpc_portname))
    {
        yCError(CONTROLBOARD, "Failed to open port %s", server_rpc_portname.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    std::string rootName = m_name;

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
        setPeriod(m_period);
        if (!start()) {
            yCError(CONTROLBOARD) << "Error starting thread";
            return false;
        }
    }

    return true;
}

bool ControlBoard_nws_yarp::setDevice(yarp::dev::DeviceDriver* driver, bool owned)
{
    // Save the pointer and subDeviceOwned
    yarp::dev::DeviceDriver* subdevice_ptr = driver;

    subdevice_ptr->view(iJointFault);
    if (!iJointFault) {
        yCWarning(CONTROLBOARD, "Part <%s>: iJointFault interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iPidControl);
    if (!iPidControl) {
        yCWarning(CONTROLBOARD, "Part <%s>: IPidControl interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iPositionControl);
    if (!iPositionControl) {
        yCWarning(CONTROLBOARD, "Part <%s>: IPositionControl interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iPositionDirect);
    if (!iPositionDirect) {
        yCWarning(CONTROLBOARD, "Part <%s>: IPositionDirect interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iVelocityControl);
    if (!iVelocityControl) {
        yCWarning(CONTROLBOARD, "Part <%s>: IVelocityControl interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iEncodersTimed);
    if (!iEncodersTimed) {
        yCWarning(CONTROLBOARD, "Part <%s>: IEncodersTimed interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iMotor);
    if (!iMotor) {
        yCWarning(CONTROLBOARD, "Part <%s>: IMotor interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iMotorEncoders);
    if (!iMotorEncoders) {
        yCWarning(CONTROLBOARD, "Part <%s>: IMotorEncoders interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iAmplifierControl);
    if (!iAmplifierControl) {
        yCWarning(CONTROLBOARD, "Part <%s>: IAmplifierControl interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iControlLimits);
    if (!iControlLimits) {
        yCWarning(CONTROLBOARD, "Part <%s>: IControlLimits interface was not found in subdevice.", partName.c_str());
    }

     subdevice_ptr->view(iControlCalibration);
    if (!iControlCalibration) {
        yCWarning(CONTROLBOARD, "Part <%s>: IControlCalibration interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iTorqueControl);
    if (!iTorqueControl) {
        yCWarning(CONTROLBOARD, "Part <%s>: ITorqueControl interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iImpedanceControl);
    if (!iImpedanceControl) {
        yCWarning(CONTROLBOARD, "Part <%s>: IImpedanceControl interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iControlMode);
    if (!iControlMode) {
        yCWarning(CONTROLBOARD, "Part <%s>: IControlMode interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iAxisInfo);
    if (!iAxisInfo) {
        yCWarning(CONTROLBOARD, "Part <%s>: IAxisInfo interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iPreciselyTimed);
    if (!iPreciselyTimed) {
        yCWarning(CONTROLBOARD, "Part <%s>: IPreciselyTimed interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iInteractionMode);
    if (!iInteractionMode) {
        yCWarning(CONTROLBOARD, "Part <%s>: IInteractionMode interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iRemoteVariables);
    if (!iRemoteVariables) {
        yCWarning(CONTROLBOARD, "Part <%s>: IRemoteVariables interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iPWMControl);
    if (!iPWMControl) {
        yCWarning(CONTROLBOARD, "Part <%s>: IPWMControl interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iCurrentControl);
    if (!iCurrentControl) {
        yCWarning(CONTROLBOARD, "Part <%s>: ICurrentControl interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iJointBrake);
    if (!iJointBrake) {
        yCWarning(CONTROLBOARD, "Part <%s>: iJointBrake interface was not found in subdevice.", partName.c_str());
    }

    subdevice_ptr->view(iVelocityDirect);
    if (!iVelocityDirect) {
        yCWarning(CONTROLBOARD, "Part <%s>: iVelocityDirect interface was not found in subdevice.", partName.c_str());
    }

    // Get the number of controlled joints
    int tmp_axes = 0;
    if (iAxisInfo)
    {
        if (!iAxisInfo->getAxes(&tmp_axes)) {
            yCError(CONTROLBOARD) << "Part <%s>: iAxisInfo->getAxes() failed for subdevice " << partName.c_str();
            return false;
        }
    }
    else if (iEncodersTimed)
    {
        if (!iEncodersTimed->getAxes(&tmp_axes)) {
            yCError(CONTROLBOARD) << "Part <%s>: iEncodersTimed->getAxes() failed for subdevice " << partName.c_str();
            return false;
        }
    }
    else if (iPositionControl)
    {
        if (!iPositionControl->getAxes(&tmp_axes)) {
            yCError(CONTROLBOARD) << "Part <%s>: iPositionControl->getAxes() failed for subdevice " << partName.c_str();
            return false;
        }
    }
    else if (iVelocityControl)
    {
        if (!iVelocityControl->getAxes(&tmp_axes)) {
            yCError(CONTROLBOARD) << "Part <%s>: iVelocityControl->getAxes() failed for subdevice " << partName.c_str();
            return false;
        }
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
    iJointFault = nullptr;
    iVelocityDirect = nullptr;
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

    setPeriod(m_period);
    if (!start()) {
        yCError(CONTROLBOARD) << "Error starting thread";
        return false;
    }

    m_RPC = std::make_unique<ControlBoardRPCd>(this->subdevice_joints, iJointBrake, iVelocityDirect, iPidControl, iControlLimits, iControlMode);

    return true;
}

bool ControlBoard_nws_yarp::detach()
{
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
        yCIWarning(CONTROLBOARD, id()) << "Number of streaming input messages to be read is " << inputStreamingPort.getPendingReads() << " and can overflow";
    }
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
    data.temperature.resize(subdevice_joints);
    data.current.resize(subdevice_joints);
    data.controlMode.resize(subdevice_joints);
    data.interactionMode.resize(subdevice_joints);

    // resize the temporary vector
    tmpVariableForFloatSignals.resize(subdevice_joints);

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

    if (iMotor) {
        // the temperature is a double in the interface, but a float in the jointData struct.
        data.temperature_isValid = iMotor->getTemperatures(tmpVariableForFloatSignals.data());

        // The temperature values are stored as double in the interface but need to be converted to float in the jointData struct.
        // We manually copy them, and while this conversion may lead to a minor loss of precision, it is negligible for temperature values,
        // which typically do not require double precision. std::copy ensures a safe and efficient conversion.
        std::copy(tmpVariableForFloatSignals.begin(), tmpVariableForFloatSignals.end(), data.temperature.begin());
    } else {
        data.temperature_isValid = false;
    }

    if (iTorqueControl) {
        data.torque_isValid = iTorqueControl->getTorques(data.torque.data());
    } else {
        data.torque_isValid = false;
    }

    if (iPWMControl) {
        // the pwmDutycycle is a double in the interface, but a float in the jointData struct.
        data.pwmDutycycle_isValid = iPWMControl->getDutyCycles(tmpVariableForFloatSignals.data());

        // The pwmDutycycle values are stored as double in the interface but need to be converted to float in the jointData struct.
        // We manually copy them, and while this conversion may lead to a minor loss of precision, it is negligible for pwmDutycycle values,
        // which typically do not require double precision. std::copy ensures a safe and efficient conversion.
        std::copy(tmpVariableForFloatSignals.begin(), tmpVariableForFloatSignals.end(), data.pwmDutycycle.begin());
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

    // Check if the encoders timestamps are consistent.
    for (double tt : times)
    {
        if (std::abs(times[0] - tt) > 1.0)
        {
            yCIErrorThrottle(CONTROLBOARD, id(), 1.0) << "Encoder timestamps are not consistent! Data will not be published.";
            return;
        }
    }

    // Update the port envelope time by averaging all timestamps
    time.update(std::accumulate(times.begin(), times.end(), 0.0) / subdevice_joints);
    yarp::os::Stamp averageTime = time;

    extendedOutputStatePort.enableBackgroundWrite(true);
    extendedOutputStatePort.setEnvelope(averageTime);
    extendedOutputState_buffer.write();

    // handle state:o
    yarp::sig::Vector& v = outputPositionStatePort.prepare();
    v.resize(subdevice_joints);
    std::copy(data.jointPosition.begin(), data.jointPosition.end(), v.begin());

    outputPositionStatePort.setEnvelope(averageTime);
    outputPositionStatePort.write();
}

bool ControlBoard_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    if (!connection.isValid()) {
        return false;
    }
    if (!m_RPC) {
        return false;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    bool b = m_RPC->read(connection);
    if (b) {
        return true;
    }
    yCDebug(CONTROLBOARD) << "read() Command failed";
    return false;
}
