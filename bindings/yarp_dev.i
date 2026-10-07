// SPDX-FileCopyrightText: 2006-2026 Istituto Italiano di Tecnologia (IIT)
// SPDX-License-Identifier: BSD-3-Clause

//////////////////////////////////////////////////////////////////////////
//
// SWIG wrapping for the YARP_dev library.
//
// This file is not a standalone module: it is meant to be included by
// yarp.i, after YARP_os and YARP_sig have been wrapped and after the
// std::vector templates (DVector, IVector, ...) have been instantiated.

%{
#include <yarp/dev/api.h>
%}

%import <yarp/dev/api.h>

%feature("notabstract") ReturnValue;

// Deal with overridden method clashes, simply by ignoring them.
%ignore *::setKp(double);
%ignore *::setKi(double);
%ignore *::setKd(double);
%ignore *::setKff(double);
%ignore *::setScale(double);
%ignore *::setOffset(double);
%rename(open_str) yarp::dev::PolyDriver::open(const std::string& txt);

%include <yarp/dev/DeviceDriver.h>
%include <yarp/dev/PolyDriver.h>
%include <yarp/dev/Drivers.h>
%include <yarp/dev/ReturnValue.h>
%include <yarp/dev/IFrameGrabberImage.h>
%include <yarp/dev/IFrameGrabberControls.h>
%include <yarp/dev/IFrameGrabberControlsDC1394.h>
%include <yarp/dev/IFrameWriterImage.h>
%include <yarp/dev/AudioVisualInterfaces.h>
%include <yarp/dev/ControlBoardInterfaces.h>
%include <yarp/dev/IAxisInfo.h>
%include <yarp/dev/IAmplifierControl.h>
%include <yarp/dev/IControlDebug.h>
%include <yarp/dev/IControlLimits.h>
%include <yarp/dev/ControlBoardPid.h>
%include <yarp/dev/CartesianControl.h>
%include <yarp/dev/GazeControl.h>
%include <yarp/dev/IPositionControl.h>
%include <yarp/dev/IEncoders.h>
%include <yarp/dev/CalibratorInterfaces.h>
%include <yarp/dev/ControlBoardPid.h>
%include <yarp/dev/IControlMode.h>
%include <yarp/dev/IInteractionMode.h>
%include <yarp/dev/IJointCoupling.h>
%include <yarp/dev/IJointFault.h>
%include <yarp/dev/IEncodersTimed.h>
%include <yarp/dev/IMotor.h>
%include <yarp/dev/IMotorEncoders.h>
%include <yarp/dev/ITorqueControl.h>
%include <yarp/dev/IImpedanceControl.h>
%include <yarp/dev/IVelocityControl.h>
%include <yarp/dev/IPWMControl.h>
%include <yarp/dev/ICurrentControl.h>
%include <yarp/dev/IAnalogSensor.h>
%include <yarp/dev/IRemoteVariables.h>
%include <yarp/dev/IPidControl.h>
%include <yarp/dev/IPositionDirect.h>
%include <yarp/dev/ISpeechSynthesizer.h>
%include <yarp/dev/ISpeechTranscription.h>
%include <yarp/dev/LLM_Message.h>
%include <yarp/dev/ILLM.h>
%include <yarp/dev/MultipleAnalogSensorsInterfaces.h>
%include <yarp/dev/CameraConfigData.h>
%include <yarp/dev/IRgbVisualParams.h>
%include <yarp/dev/IDepthVisualParams.h>
%include <yarp/dev/IRGBDSensor.h>
%include <yarp/dev/IBattery.h>
%include <yarp/dev/ISimulatedWorld.h>

#if !defined(YARP_NO_MATH)
%include <yarp/dev/IFrameTransform.h>
%include <yarp/dev/ILocalization2D.h>
%include <yarp/dev/IMap2D.h>
%include <yarp/dev/INavigation2D.h>
%include <yarp/dev/Map2DLocationData.h>
%include <yarp/dev/Map2DObjectData.h>
%include <yarp/dev/Map2DPathData.h>
%include <yarp/dev/Map2DAreaData.h>
%include <yarp/dev/Map2DLocation.h>
%include <yarp/dev/Map2DObject.h>
%include <yarp/dev/Map2DPath.h>
%include <yarp/dev/Map2DArea.h>
%include <yarp/dev/MapGrid2D.h>
%include <yarp/dev/NavTypes.h>
#endif

%template(LLMVector) std::vector<yarp::dev::LLM_Message>;
%template(CameraConfigVector) std::vector<yarp::dev::CameraConfigData>;

#if !defined(YARP_NO_MATH)
%template(Map2DLocationVector) std::vector<yarp::dev::Nav2D::Map2DLocation>;
#endif

#if SWIG_VERSION < 0x040201
#if defined(SWIGCSHARP)
  SWIG_STD_VECTOR_SPECIALIZE_MINIMUM(Pid,yarp::dev::Pid)
#endif
#endif
%template(PidVector) std::vector<yarp::dev::Pid>;

%{
#if !defined(YARP_NO_MATH)
typedef yarp::dev::Nav2D::Map2DLocation Map2DLocation;
typedef yarp::dev::Nav2D::MapGrid2D MapGrid2D;
typedef yarp::dev::Nav2D::Map2DArea Map2DArea;
typedef yarp::dev::Nav2D::Map2DPath Map2DPath;
typedef yarp::dev::Nav2D::XYCell XYCell;
typedef yarp::dev::Nav2D::XYWorld XYWorld;
#endif
%}

#if !defined(YARP_NO_MATH)
MAKE_COMMS  (Map2DLocation, yarp::dev::Nav2D::Map2DLocation)
MAKE_COMMS  (MapGrid2D, yarp::dev::Nav2D::MapGrid2D)
MAKE_COMMS  (Map2DArea, yarp::dev::Nav2D::Map2DArea)
MAKE_COMMS  (Map2DPath, yarp::dev::Nav2D::Map2DPath)
#endif

//////////////////////////////////////////////////////////////////////////
// Deal with PolyDriver idiom that doesn't translate too well

%define CAST_POLYDRIVER_TO_INTERFACE(interface)
    yarp::dev:: ## interface *view ## interface ## () {
        yarp::dev:: ## interface *result;
        self->view(result);
        return result;
    }
%enddef

// Macro for interfaces in namespaces (e.g., yarp::dev::Nav2D::INavigation2D)
// Requires full class path and method name suffix
%extend yarp::dev::PolyDriver {

    CAST_POLYDRIVER_TO_INTERFACE(IFrameGrabberImage)
    CAST_POLYDRIVER_TO_INTERFACE(IPositionControl)
    CAST_POLYDRIVER_TO_INTERFACE(IVelocityControl)
    CAST_POLYDRIVER_TO_INTERFACE(IEncoders)
    CAST_POLYDRIVER_TO_INTERFACE(IEncodersTimed)
    CAST_POLYDRIVER_TO_INTERFACE(IMotor)
    CAST_POLYDRIVER_TO_INTERFACE(IMotorEncoders)
    CAST_POLYDRIVER_TO_INTERFACE(IPidControl)
    CAST_POLYDRIVER_TO_INTERFACE(IAmplifierControl)
    CAST_POLYDRIVER_TO_INTERFACE(IControlLimits)
    CAST_POLYDRIVER_TO_INTERFACE(ICartesianControl)
    CAST_POLYDRIVER_TO_INTERFACE(IGazeControl)
    CAST_POLYDRIVER_TO_INTERFACE(IImpedanceControl)
    CAST_POLYDRIVER_TO_INTERFACE(ITorqueControl)
    CAST_POLYDRIVER_TO_INTERFACE(IControlMode)
    CAST_POLYDRIVER_TO_INTERFACE(IJointCoupling)
    CAST_POLYDRIVER_TO_INTERFACE(IJointFault)
    CAST_POLYDRIVER_TO_INTERFACE(IInteractionMode)
    CAST_POLYDRIVER_TO_INTERFACE(IPWMControl)
    CAST_POLYDRIVER_TO_INTERFACE(ICurrentControl)
    CAST_POLYDRIVER_TO_INTERFACE(IAnalogSensor)
    CAST_POLYDRIVER_TO_INTERFACE(IFrameGrabberControls)
    CAST_POLYDRIVER_TO_INTERFACE(IPositionDirect)
    CAST_POLYDRIVER_TO_INTERFACE(IRemoteVariables)
    CAST_POLYDRIVER_TO_INTERFACE(IAxisInfo)
    CAST_POLYDRIVER_TO_INTERFACE(ISpeechSynthesizer)
    CAST_POLYDRIVER_TO_INTERFACE(ISpeechTranscription)
    CAST_POLYDRIVER_TO_INTERFACE(ILLM)
    CAST_POLYDRIVER_TO_INTERFACE(IRGBDSensor)
    CAST_POLYDRIVER_TO_INTERFACE(IBattery)
    CAST_POLYDRIVER_TO_INTERFACE(ISimulatedWorld)

#if !defined(YARP_NO_MATH)
    CAST_POLYDRIVER_TO_INTERFACE(IFrameTransform)
    // View methods for Nav2D interfaces
    yarp::dev::Nav2D::INavigation2D *viewINavigation2D() {
        yarp::dev::Nav2D::INavigation2D *result;
        self->view(result);
        return result;
    }

    yarp::dev::Nav2D::ILocalization2D *viewILocalization2D() {
        yarp::dev::Nav2D::ILocalization2D *result;
        self->view(result);
        return result;
    }

    yarp::dev::Nav2D::IMap2D *viewIMap2D() {
        yarp::dev::Nav2D::IMap2D *result;
        self->view(result);
        return result;
    }
#endif

// These views are currently disabled in SWIG + java generator since they are
// useless without the EXTENDED_ANALOG_SENSOR_INTERFACE part.
// See also https://github.com/robotology/yarp/issues/1770
#if !defined(SWIGJAVA) && !defined(SWIGCSHARP)
    CAST_POLYDRIVER_TO_INTERFACE(IThreeAxisGyroscopes)
    CAST_POLYDRIVER_TO_INTERFACE(IThreeAxisLinearAccelerometers)
    CAST_POLYDRIVER_TO_INTERFACE(IThreeAxisMagnetometers)
    CAST_POLYDRIVER_TO_INTERFACE(IOrientationSensors)
    CAST_POLYDRIVER_TO_INTERFACE(ITemperatureSensors)
    CAST_POLYDRIVER_TO_INTERFACE(ISixAxisForceTorqueSensors)
    CAST_POLYDRIVER_TO_INTERFACE(IContactLoadCellArrays)
    CAST_POLYDRIVER_TO_INTERFACE(IEncoderArrays)
    CAST_POLYDRIVER_TO_INTERFACE(ISkinPatches)
#endif

    // you'll need to add an entry for every interface you wish
    // to use
}


//////////////////////////////////////////////////////////////////////////
// Deal with ControlBoardInterfaces pointer arguments that don't translate

%extend yarp::dev::IImpedanceControl {
    size_t getAxes() {
        size_t buffer;
        bool ok = self->getAxes(buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool getImpedance(int j, std::vector<double>& stiffness, std::vector<double>& damping) {
        return self->getImpedance(j, &stiffness[0], &damping[0]);
    }

    bool getImpedanceOffset(int j, std::vector<double>& data) {
        return self->getImpedanceOffset(j, &data[0]);
    }

    bool getCurrentImpedanceLimit(int j, std::vector<double>& min_stiff, std::vector<double>& max_stiff, std::vector<double>& min_damp, std::vector<double>& max_damp) {
        return self->getCurrentImpedanceLimit(j, &min_stiff[0], &max_stiff[0], &min_damp[0], &max_damp[0]);
    }
}

%extend yarp::dev::IPositionControl {
    size_t getAxes() {
        size_t buffer;
        bool ok = self->getAxes(buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool positionMove(std::vector<double>& data) {
        return self->positionMove(&data[0]);
    }

    bool positionMove(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->positionMove(n_joint, &joints[0], &data[0]);
    }

    bool relativeMove(std::vector<double>& data) {
        return self->relativeMove(&data[0]);
    }

    bool relativeMove(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->relativeMove(n_joint, &joints[0], &data[0]);
    }

    bool setTrajSpeeds(std::vector<double>& data) {
        return self->setTrajSpeeds(&data[0]);
    }

    bool setTrajSpeeds(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->setTrajSpeeds(n_joint, &joints[0], &data[0]);
    }

    bool getTrajSpeed(int j, std::vector<double>& data) {
        return self->getTrajSpeed(j, &data[0]);
    }

    bool getTrajSpeeds(std::vector<double>& data) {
        return self->getTrajSpeeds(&data[0]);
    }

    bool getTrajSpeeds(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->getTrajSpeeds(n_joint, &joints[0], &data[0]);
    }

    bool setTrajAccelerations(std::vector<double>& data) {
        return self->setTrajAccelerations(&data[0]);
    }

    bool setTrajAccelerations(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->setTrajAccelerations(n_joint, &joints[0], &data[0]);
    }

    bool getTrajAcceleration(int j, std::vector<double>& data) {
        return self->getTrajAcceleration(j, &data[0]);
    }

    bool getTrajAccelerations(std::vector<double>& data) {
        return self->getTrajAccelerations(&data[0]);
    }

    bool getTrajAccelerations(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->getTrajAccelerations(n_joint, &joints[0], &data[0]);
    }


    bool checkMotionDone(int joint, bool& ok) {
        return self->checkMotionDone(joint, ok);
    }

    bool checkMotionDone(bool& ok) {
        return self->checkMotionDone(ok);
    }

    bool checkMotionDone(std::vector<int> joints, bool& ok) {
        return self->checkMotionDone(joints, ok);
    }

    bool stop(int n_joint, std::vector<int>& joints) {
        return self->stop(n_joint, &joints[0]);
    }

    bool getTargetPosition(int j, std::vector<double>& data) {
        return self->getTargetPosition(j, &data[0]);
    }

    bool getTargetPositions(std::vector<double>& data) {
        return self->getTargetPositions(&data[0]);
    }

    bool getTargetPositions(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->getTargetPositions(n_joint, &joints[0], &data[0]);
    }
}

%extend yarp::dev::IVelocityControl {
    size_t getAxes() {
        size_t buffer;
        bool ok = self->getAxes(buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool velocityMove(std::vector<double>& data) {
        return self->velocityMove(&data[0]);
    }

    bool setTrajAccelerations(std::vector<double>& data) {
        return self->setTrajAccelerations(&data[0]);
    }

    bool getTrajAcceleration(int j, std::vector<double>& data) {
        return self->getTrajAcceleration(j, &data[0]);
    }

    bool getTrajAccelerations(std::vector<double>& data) {
        return self->getTrajAccelerations(&data[0]);
    }

    bool velocityMove(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->velocityMove(n_joint, &joints[0], &data[0]);
    }

    bool getTargetVelocity(int j, std::vector<double>& data) {
        return self->getTargetVelocity(j, &data[0]);
    }

    bool getTargetVelocities(std::vector<double>& data) {
        return self->getTargetVelocities(&data[0]);
    }

    bool getTargetVelocities(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->getTargetVelocities(n_joint, &joints[0], &data[0]);
    }

    bool setTrajAccelerations(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->setTrajAccelerations(n_joint, &joints[0], &data[0]);
    }

    bool getTrajAccelerations(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->getTrajAccelerations(n_joint, &joints[0], &data[0]);
    }

    bool stop(int n_joint, std::vector<int>& joints) {
        return self->stop(n_joint, &joints[0]);
    }
}

%extend yarp::dev::IEncoders {
    size_t getAxes() {
        size_t buffer;
        bool ok = self->getAxes(buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool setEncoders(std::vector<double>& data) {
        return self->setEncoders(&data[0]);
    }

    double getEncoder(int j) {
        double data;
        bool ok = self->getEncoder(j, &data);
        if (!ok) return 0;
        return data;
    }

    bool getEncoders(std::vector<double>& data) {
        return self->getEncoders(&data[0]);
    }

    double getEncoderSpeed(int j) {
        double data;
        bool ok = self->getEncoderSpeed(j, &data);
        if (!ok) return 0;
        return data;
    }

    bool getEncoderSpeeds(std::vector<double>& data) {
        return self->getEncoderSpeeds(&data[0]);
    }

    double getEncoderAcceleration(int j) {
        double data;
        bool ok = self->getEncoderAcceleration(j, &data);
        if (!ok) return 0;
        return data;
    }

    bool getEncoderAccelerations(std::vector<double>& data) {
        return self->getEncoderAccelerations(&data[0]);
    }
}

%extend yarp::dev::IEncodersTimed {
    bool getEncodersTimed(std::vector<double>& data, std::vector<double>& time) {
        return self->getEncodersTimed(&data[0], &time[0]);
    }

    bool getEncoderTimed(int j, std::vector<double>& data, std::vector<double>& time) {
        return self->getEncoderTimed(j, &data[0], &time[0]);
    }
}

%extend yarp::dev::IMotorEncoders {
    int getNumberOfMotorEncoders() {
        int nbEncs;
        bool ok = self->getNumberOfMotorEncoders(&nbEncs);
        if (!ok) return 0;
        return nbEncs;
    }

    bool getMotorEncoderCountsPerRevolution(int j, std::vector<double>& data) {
        return self->getMotorEncoderCountsPerRevolution(j, &data[0]);
    }

    bool setMotorEncoders(std::vector<double>& encs) {
        return self->setMotorEncoders(&encs[0]);
    }

    double getMotorEncoder(int j) {
        double enc;
        bool ok = self->getMotorEncoder(j, &enc);
        if (!ok) return 0;
        return enc;
    }

    bool getMotorEncoders(std::vector<double>& encs) {
        return self->getMotorEncoders(&encs[0]);
    }

    bool getMotorEncoderTimed(int j, std::vector<double>& enc, std::vector<double>& time) {
        return self->getMotorEncoderTimed(j, &enc[0], &time[0]);
    }

    bool getMotorEncodersTimed(std::vector<double>& encs, std::vector<double>& times) {
        return self->getMotorEncodersTimed(&encs[0], &times[0]);
    }

    double getMotorEncoderSpeed(int j) {
        double speed;
        bool ok = self->getMotorEncoderSpeed(j, &speed);
        if (!ok) return 0;
        return speed;
    }

    bool getMotorEncoderSpeeds(std::vector<double>& speeds) {
        return self->getMotorEncoderSpeeds(&speeds[0]);
    }

    bool getMotorEncoderAcceleration(int j, std::vector<double>& acc) {
        return self->getMotorEncoderAcceleration(j, &acc[0]);
    }

    bool getMotorEncoderAccelerations(std::vector<double>& accs) {
        return self->getMotorEncoderAccelerations(&accs[0]);
    }
}

%extend yarp::dev::IAmplifierControl {
    bool getAmpStatus(std::vector<int>& data) {
        return self->getAmpStatus(&data[0]);
    }

    bool getAmpStatus(int j, std::vector<int>& data) {
        return self->getAmpStatus(j, &data[0]);
    }

    bool getCurrents(std::vector<double>& data) {
        return self->getCurrents(&data[0]);
    }

    bool getCurrent(int j, std::vector<double>& data) {
        return self->getCurrent(j, &data[0]);
    }

    bool getMaxCurrent(int j, std::vector<double>& data) {
        return self->getMaxCurrent(j, &data[0]);
    }

    bool getNominalCurrent(int j, std::vector<double>& data) {
        return self->getNominalCurrent(j, &data[0]);
    }

    bool getPeakCurrent(int j, std::vector<double>& data) {
        return self->getPeakCurrent(j, &data[0]);
    }

    bool getPWM(int j, std::vector<double>& data) {
        return self->getPWM(j, &data[0]);
    }

    bool getPWMLimit(int j, std::vector<double>& data) {
        return self->getPWMLimit(j, &data[0]);
    }

    bool getPowerSupplyVoltage(int j, std::vector<double>& data) {
        return self->getPowerSupplyVoltage(j, &data[0]);
    }
}

%extend yarp::dev::IControlLimits {
    bool getPosLimits(int axis, std::vector<double>& min, std::vector<double>& max) {
        return self->getPosLimits(axis, &min[0], &max[0]);
    }

    bool getVelLimits(int axis, std::vector<double>& min, std::vector<double>& max) {
        return self->getVelLimits(axis, &min[0], &max[0]);
    }
}

%extend yarp::dev::IJointFault {
    bool getLastJointFault(int j, std::vector<int>& fault, std::vector<std::string>& message) {
        return self->getLastJointFault(j, fault[0], message[0]);
    }
}

%extend yarp::dev::IJointCoupling {
    size_t getNrOfPhysicalJoints() {
        size_t nrOfPhysicalJoints;
        bool ok = self->getNrOfPhysicalJoints(nrOfPhysicalJoints);
        if (!ok) return 0;
        return nrOfPhysicalJoints;
    }

    size_t getNrOfActuatedAxes() {
        size_t nrOfActuatedAxes;
        bool ok = self->getNrOfActuatedAxes(nrOfActuatedAxes);
        if (!ok) return 0;
        return nrOfActuatedAxes;
    }

    std::string getActuatedAxisName(size_t actuatedAxisIndex) {
        std::string actuatedAxisName;
        bool ok = self->getActuatedAxisName(actuatedAxisIndex, actuatedAxisName);
        if (!ok) return "unknown";
        return actuatedAxisName;
    }

    std::string getPhysicalJointName(size_t physicalJointIndex) {
        std::string physicalJointName;
        bool ok = self->getPhysicalJointName(physicalJointIndex, physicalJointName);
        if (!ok) return "unknown";
        return physicalJointName;
    }
}

%extend yarp::dev::IControlMode {
        yarp::dev::ControlModeEnum getControlMode(int j) {
        yarp::dev::ControlModeEnum mode = yarp::dev::ControlModeEnum::VOCAB_CM_UNKNOWN;
        self->getControlMode(j, mode);
        return mode;
    }

    bool getControlModes(std::vector<yarp::dev::ControlModeEnum>& data) {
        return self->getControlModes(data);
    }

    bool getControlModes(std::vector<int>& joints, std::vector<yarp::dev::ControlModeEnum>& data) {
        return self->getControlModes(joints, data);
    }

    bool setControlModes(std::vector<yarp::dev::SelectableControlModeEnum>& data) {
        return self->setControlModes(data);
    }

    bool setControlModes(std::vector<int>& joints, std::vector<yarp::dev::SelectableControlModeEnum>& data) {
        return self->setControlModes(joints, data);
    }
}

%extend yarp::dev::IInteractionMode {
       yarp::dev::InteractionModeEnum getInteractionMode(int axis) {
       yarp::dev::InteractionModeEnum mode = yarp::dev::InteractionModeEnum::VOCAB_IM_UNKNOWN;
       self->getInteractionMode(axis, mode);
       return mode;
    }

    bool getInteractionModes(std::vector<int>& joints, std::vector<yarp::dev::InteractionModeEnum>& data) {
        return self->getInteractionModes(joints, data);
    }

    bool getInteractionModes(std::vector<yarp::dev::InteractionModeEnum>& data) {
        return self->getInteractionModes(data);
    }

    bool setInteractionModes(std::vector<int>& joints, std::vector<yarp::dev::InteractionModeEnum>& data) {
        return self->setInteractionModes(joints, data);
    }

    bool setInteractionModes(std::vector<yarp::dev::InteractionModeEnum>& data) {
        return self->setInteractionModes(data);
    }
}

%extend yarp::dev::IPositionDirect {
    size_t getAxes() {
        size_t buffer;
        bool ok = self->getAxes(buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool setPositions(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->setPositions(n_joint, &joints[0], &data[0]);
    }

    bool setPositions(std::vector<double>& data) {
        return self->setPositions(&data[0]);
    }

    bool getRefPosition(int j, std::vector<double>& data) {
        return self->getRefPosition(j, &data[0]);
    }

    bool getRefPositions(std::vector<double>& data) {
        return self->getRefPositions(&data[0]);
    }

    bool getRefPositions(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->getRefPositions(n_joint, &joints[0], &data[0]);
    }
}

%extend yarp::dev::IAxisInfo {
    std::string getAxisName(int axis) {
        std::string name;
        bool ok = self->getAxisName(axis, name);
        if (!ok) return "unknown";
        return name;
    }

    yarp::dev::JointTypeEnum getJointType(int axis) {
        yarp::dev::JointTypeEnum type;
        bool ok = self->getJointType(axis, type);
        if (!ok) return yarp::dev::JointTypeEnum::VOCAB_JOINTTYPE_UNKNOWN;
        return type;
    }
}

%extend yarp::dev::ICurrentControl {
    int getNumberOfMotors() {
        int buffer;
        bool ok = self->getNumberOfMotors(&buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool getCurrent(int j, std::vector<double>& data) {
        return self->getCurrent(j, &data[0]);
    }

    bool getCurrents(std::vector<double>& data) {
        return self->getCurrents(&data[0]);
    }

    bool getCurrentRange(int j, std::vector<double>& min, std::vector<double>& max) {
        return self->getCurrentRange(j, &min[0], &max[0]);
    }

    bool getCurrentRanges(std::vector<double>& mins, std::vector<double>& maxs) {
        return self->getCurrentRanges(&mins[0], &maxs[0]);
    }

    bool setRefCurrents(std::vector<double>& data) {
        return self->setRefCurrents(&data[0]);
    }

    bool setRefCurrents(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->setRefCurrents(n_joint, &joints[0], &data[0]);
    }

    bool getRefCurrents(std::vector<double>& data) {
        return self->getRefCurrents(&data[0]);
    }

    bool getRefCurrent(int j, std::vector<double>& data) {
        return self->getRefCurrent(j, &data[0]);
    }
}

%extend yarp::dev::IMotor {
    int getNumberOfMotors() {
        int buffer;
        bool ok = self->getNumberOfMotors(&buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool getTemperature(int j, std::vector<double>& data) {
        return self->getTemperature(j, &data[0]);
    }

    bool getTemperatures(std::vector<double>& data) {
        return self->getTemperatures(&data[0]);
    }

    bool getTemperatureLimit(int j, std::vector<double>& data) {
        return self->getTemperatureLimit(j, &data[0]);
    }

    bool getGearboxRatio(int j, std::vector<double>& data) {
        return self->getGearboxRatio(j, &data[0]);
    }
}

%extend yarp::dev::IPWMControl {
    int getNumberOfMotors() {
        int buffer;
        bool ok = self->getNumberOfMotors(&buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool setRefDutyCycles(std::vector<double>& data) {
        return self->setRefDutyCycles(&data[0]);
    }

    bool getRefDutyCycle(int j, std::vector<double>& data) {
        return self->getRefDutyCycle(j, &data[0]);
    }

    bool getRefDutyCycles(std::vector<double>& data) {
        return self->getRefDutyCycles(&data[0]);
    }

    bool getDutyCycle(int j, std::vector<double>& data) {
        return self->getDutyCycle(j, &data[0]);
    }

    bool getDutyCycles(std::vector<double>& data) {
        return self->getDutyCycles(&data[0]);
    }
}

%extend yarp::dev::ITorqueControl {
    size_t getAxes() {
        size_t buffer;
        bool ok = self->getAxes(buffer);
        if (!ok) return 0;
        return buffer;
    }

    bool getRefTorques(std::vector<double>& data) {
        return self->getRefTorques(&data[0]);
    }

    bool getRefTorque(int j, std::vector<double>& data) {
        return self->getRefTorque(j, &data[0]);
    }

    bool setRefTorques(std::vector<double>& data) {
        return self->setRefTorques(&data[0]);
    }

    bool setRefTorques(int n_joint, std::vector<int>& joints, std::vector<double>& data) {
        return self->setRefTorques(n_joint, &joints[0], &data[0]);
    }

    bool getMotorTorqueParams(int j, yarp::dev::MotorTorqueParameters& params) {
        return self->getMotorTorqueParams(j, &params);
    }

    bool getTorque(int j, std::vector<double>& data) {
        return self->getTorque(j, &data[0]);
    }

    bool getTorques(std::vector<double>& data) {
        return self->getTorques(&data[0]);
    }

    bool getTorqueRange(int j, std::vector<double>& min, std::vector<double>& max) {
        return self->getTorqueRange(j, &min[0], &max[0]);
    }

    bool getTorqueRanges(std::vector<double>& mins, std::vector<double>& maxs) {
        return self->getTorqueRanges(&mins[0], &maxs[0]);
    }
}

%extend yarp::dev::IPidControl {
    bool setPid(int pidtype, int j, const yarp::dev::Pid& pid) {
        return self->setPid((yarp::dev::PidControlTypeEnum)pidtype, j, pid);
    }

    bool setPids(int pidtype, std::vector<yarp::dev::Pid>& pids) {
        return self->setPids((yarp::dev::PidControlTypeEnum)pidtype, &pids[0]);
    }

    bool setPidReference(int pidtype, int j, double ref) {
        return self->setPidReference((yarp::dev::PidControlTypeEnum)pidtype, j, ref);
    }

    bool setPidReferences(int pidtype, std::vector<double>& data) {
        return self->setPidReferences((yarp::dev::PidControlTypeEnum)pidtype, &data[0]);
    }

    bool setPidErrorLimit(int pidtype, int j, double limit) {
        return self->setPidErrorLimit((yarp::dev::PidControlTypeEnum)pidtype, j, limit);
    }

    bool setPidErrorLimits(int pidtype, std::vector<double>& data) {
        return self->setPidErrorLimits((yarp::dev::PidControlTypeEnum)pidtype, &data[0]);
    }

    bool getPidError(int pidtype, int j, std::vector<double>& data) {
        return self->getPidError((yarp::dev::PidControlTypeEnum)pidtype, j, &data[0]);
    }

    bool getPidErrors(int pidtype, std::vector<double>& data) {
        return self->getPidErrors((yarp::dev::PidControlTypeEnum)pidtype, &data[0]);
    }

    bool getPidOutput(int pidtype, int j, std::vector<double>& data) {
        return self->getPidOutput((yarp::dev::PidControlTypeEnum)pidtype, j, &data[0]);
    }

    bool getPidOutputs(int pidtype, std::vector<double>& data) {
        return self->getPidOutputs((yarp::dev::PidControlTypeEnum)pidtype, &data[0]);
    }

    bool getPid(int pidtype, int j, std::vector<yarp::dev::Pid>& data) {
        return self->getPid((yarp::dev::PidControlTypeEnum)pidtype, j, &data[0]);
    }

    bool getPids(int pidtype, std::vector<yarp::dev::Pid>& data) {
        return self->getPids((yarp::dev::PidControlTypeEnum)pidtype, &data[0]);
    }

    bool getPidReference(int pidtype, int j, std::vector<double>& data) {
        return self->getPidReference((yarp::dev::PidControlTypeEnum)pidtype, j, &data[0]);
    }

    bool getPidReferences(int pidtype, std::vector<double>& data) {
        return self->getPidReferences((yarp::dev::PidControlTypeEnum)pidtype, &data[0]);
    }

    bool getPidErrorLimit(int pidtype, int j, std::vector<double>& data) {
        return self->getPidErrorLimit((yarp::dev::PidControlTypeEnum)pidtype, j, &data[0]);
    }

    bool getPidErrorLimits(int pidtype, std::vector<double>& data) {
        return self->getPidErrorLimits((yarp::dev::PidControlTypeEnum)pidtype, &data[0]);
    }

    bool resetPid(int pidtype, int j) {
        return self->resetPid((yarp::dev::PidControlTypeEnum)pidtype, j);
    }

    bool disablePid(int pidtype, int j) {
        return self->disablePid((yarp::dev::PidControlTypeEnum)pidtype, j);
    }

    bool enablePid(int pidtype, int j) {
        return self->enablePid((yarp::dev::PidControlTypeEnum)pidtype, j);
    }

    bool setPidOffset(int pidtype, int j, double offset) {
        return self->setPidOffset((yarp::dev::PidControlTypeEnum)pidtype, j, offset);
    }

    bool isPidEnabled(int pidtype, int j, bool& flag) {
        return self->isPidEnabled((yarp::dev::PidControlTypeEnum)pidtype, j, flag);
    }
}

%extend yarp::dev::ISpeechSynthesizer {
    bool getLanguage(std::vector<string>& language) {
        return self->getLanguage(language[0]);
    }

    bool getVoice(std::vector<string>& voice) {
        return self->getVoice(voice[0]);
    }

    bool getSpeed(std::vector<double>& speed) {
        return self->getSpeed(speed[0]);
    }

    bool getPitch(std::vector<double>& pitch) {
        return self->getPitch(pitch[0]);
    }
}

%extend yarp::dev::ISpeechTranscription {
    bool getLanguage(std::vector<string>& language) {
        return self->getLanguage(language[0]);
    }

    bool transcribe(const yarp::sig::Sound& sound, std::vector<string>& transcription, std::vector<double>& score) {
        return self->transcribe(sound, transcription[0], score[0]);
    }
}

%extend yarp::dev::ILLM {
    bool readPrompt(std::vector<string>& oPropmt) {
        return self->readPrompt(oPropmt[0]);
    }
}

%extend yarp::dev::ISimulatedWorld {
    std::vector<std::string> getList() {
        std::vector<std::string> names;
        bool ok = self->getList(names);
        if (!ok) return std::vector<std::string>();
        return names;
    }

    yarp::sig::Pose6D getPose(std::string id, std::string frame_name="") {
        yarp::sig::Pose6D pose;
        self->getPose(id, pose, frame_name);
        return pose;
    }
}

#if !defined(YARP_NO_MATH)
%extend yarp::dev::IFrameTransform {
    std::string allFramesAsString() {
        std::string outputString;
        bool ok = self->allFramesAsString(outputString);
        if (!ok) return "";
        return outputString;
    }

    std::vector<std::string> getAllFrameIds() {
        std::vector<std::string> frameIds;
        bool ok = self->getAllFrameIds(frameIds);
        if (!ok) return std::vector<std::string>();
        return frameIds;
    }

    std::string getParent(const std::string& frameId) {
        std::string parent;
        bool ok = self->getParent(frameId, parent);
        if (!ok) return "unknown";
        return parent;
    }

    bool canTransform(const std::string& sourceFrame, const std::string& targetFrame) {
        bool canTransform;
        bool ok = self->canTransform(sourceFrame, targetFrame, canTransform);
        if (!ok) return false;
        return canTransform;
    }

    bool frameExists(const std::string& frameId) {
        bool frameExists;
        bool ok = self->frameExists(frameId, frameExists);
        if (!ok) return false;
        return frameExists;
    }

    bool getTransform(const std::string& src, const std::string dest, yarp::sig::Matrix mat){
        bool ok = self->getTransform(src, dest, mat);
        return ok;
    }
}
#endif

%extend yarp::dev::IBattery {
    double getBatteryVoltage() {
        double voltage;
        bool ok = self->getBatteryVoltage(voltage);
        if (!ok) return 0;
        return voltage;
    }

    double getBatteryCurrent() {
        double current;
        bool ok = self->getBatteryCurrent(current);
        if (!ok) return 0;
        return current;
    }

    double getBatteryCharge() {
        double charge;
        self->getBatteryCharge(charge);
        return charge;
    }

    yarp::dev::IBattery::Battery_status getBatteryStatus() {
        yarp::dev::IBattery::Battery_status status;
        bool ok = self->getBatteryStatus(status);
        if (!ok) return yarp::dev::IBattery::Battery_status::BATTERY_GENERAL_ERROR;
        return status;
    }

    double getBatteryTemperature() {
        double temperature;
        bool ok = self->getBatteryTemperature(temperature);
        if (!ok) return 0;
        return temperature;
    }

    std::string getBatteryInfo() {
        std::string info;
        bool ok = self->getBatteryInfo(info);
        if (!ok) return "";
        return info;
    }
}

// This is part is currently broken in SWIG + java generator since SWIG 3.0.3
// (last swig version tested: 3.0.12)
// See also https://github.com/robotology/yarp/issues/1770
#if !defined(SWIGJAVA) && !defined(SWIGCSHARP)
    %extend yarp::dev::IThreeAxisGyroscopes {EXTENDED_ANALOG_SENSOR_INTERFACE(ThreeAxisGyroscope)}
    %extend yarp::dev::IThreeAxisLinearAccelerometers {EXTENDED_ANALOG_SENSOR_INTERFACE(ThreeAxisLinearAccelerometer)}
    %extend yarp::dev::IThreeAxisMagnetometers {EXTENDED_ANALOG_SENSOR_INTERFACE(ThreeAxisMagnetometer)}
    %extend yarp::dev::IOrientationSensors {EXTENDED_ANALOG_SENSOR_INTERFACE(OrientationSensor)}
    %extend yarp::dev::ITemperatureSensors {EXTENDED_ANALOG_SENSOR_INTERFACE(TemperatureSensor)}
    %extend yarp::dev::ISixAxisForceTorqueSensors {EXTENDED_ANALOG_SENSOR_INTERFACE(SixAxisForceTorqueSensor)}
    %extend yarp::dev::IContactLoadCellArrays {EXTENDED_ANALOG_SENSOR_INTERFACE(ContactLoadCellArray)}
    %extend yarp::dev::IEncoderArrays {EXTENDED_ANALOG_SENSOR_INTERFACE(EncoderArray)}
    %extend yarp::dev::ISkinPatches {EXTENDED_ANALOG_SENSOR_INTERFACE(SkinPatch)}
#endif

%extend yarp::dev::ICartesianControl {
    bool checkMotionDone(std::vector<bool>& flag) {
      std::vector<char> data(flag.size());
      bool result = self->checkMotionDone((bool*)(&data[0]));
      for (size_t i=0; i<data.size(); i++) {
        flag[i] = data[i]!=0;
      }
      return result;
    }

    bool checkMotionDone() {
        bool flag;
        if(self->checkMotionDone(&flag)) {
            return flag;
        } else {
            return false;
        }
    }

    bool isMotionDone() {
        bool data = true;
        self->checkMotionDone(&data);
        return data;
    }

    int storeContext() {
        // bad id to return if the real
        // storeContext returns false
        int badContextId = -1000;
        int ret = badContextId;
        // call the real storeContext
        bool ok = self->storeContext(&ret);
        // if not ok, return the badContextId
        if( !ok ) {
            ret = badContextId;
        }

        return ret;
    }
}

%extend yarp::dev::IGazeControl {

    bool getTrackingMode() {
        bool flag;

        if(self->getTrackingMode(&flag)) {
            return flag;
        } else {
            return false; //Not sure what is best to assume here...
        }
    }

    double getNeckTrajTime() {
        double result;

        if(self->getNeckTrajTime(&result)) {
            return result;
        } else {
            return -1.0; //On error return -1.0
        }
    }

    double getEyesTrajTime() {
        double result;

        if(self->getEyesTrajTime(&result)) {
            return result;
        } else {
            return -1.0; //On error return -1.0
        }
    }

    bool checkMotionDone() {
        bool flag;

        if(self->checkMotionDone(&flag)) {
            return flag;
        } else {
            return false;
        }
    }

    int storeContext() {
        int id;
        int badContextId = -1000;

        if(self->storeContext(&id)) {
            return id;
        } else {
            return badContextId; //On error return the badContextId
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// Deal with IFrameGrabberControls pointer arguments that don't translate
%extend yarp::dev::IFrameGrabberControls {
    CameraDescriptor getCameraDescription() {
        CameraDescriptor result;
        self->getCameraDescription(result);
        return result;
    }

    bool hasFeature(cameraFeature_id_t feature) {
        bool result;
        self->hasFeature(feature, result);
        return result;
    }

    double getFeature(cameraFeature_id_t feature) {
        double result;
        self->getFeature(feature, result);
        return result;
    }

    bool getFeature(cameraFeature_id_t j, std::vector<double>& value1, std::vector<double>& value2) {
        return self->getFeature(j, value1[0], value2[0]);
    }

    bool hasOnOff(cameraFeature_id_t feature) {
        bool result;
        self->hasOnOff(feature, result);
        return result;
    }

    bool getActive(cameraFeature_id_t feature) {
        bool result;
        self->getActive(feature, result);
        return result;
    }

    bool hasAuto(cameraFeature_id_t feature) {
        bool result;
        self->hasAuto(feature, result);
        return result;
    }

    bool hasManual(cameraFeature_id_t feature) {
        bool result;
        self->hasManual(feature, result);
        return result;
    }

    bool hasOnePush(cameraFeature_id_t feature) {
        bool result;
        self->hasOnePush(feature, result);
        return result;
    }

    FeatureMode getMode(cameraFeature_id_t feature) {
        FeatureMode result;
        self->getMode(feature, result);
        return result;
    }
}

//////////////////////////////////////////////////////////////////////////
// Adding IRGBDSensor
%extend yarp::dev::IRGBDSensor {
    bool getRgbImage(yarp::sig::FlexImage& rgbImage) {
        yarp::os::Stamp timeStamp;
        bool ok = self->getRgbImage(rgbImage, &timeStamp);
        return ok;
    }

    yarp::dev::ReturnValue getLastErrorMsg(std::vector<std::string>& message, yarp::os::Stamp* timeStamp = nullptr) {
        message.resize(1);
        return self->getLastErrorMsg(message[0], timeStamp);
    }

    yarp::dev::IRGBDSensor::RGBDSensor_status getSensorStatus() {
        yarp::dev::IRGBDSensor::RGBDSensor_status status;
        self->getSensorStatus(status);
        return status;
    }
}

//////////////////////////////////////////////////////////////////////////
// Adding IRgbVisualParams (one-element vectors as output holders)
%extend yarp::dev::IRgbVisualParams {
    yarp::dev::ReturnValue getRgbResolution(std::vector<int>& width, std::vector<int>& height) {
        width.resize(1);
        height.resize(1);
        return self->getRgbResolution(width[0], height[0]);
    }

    yarp::dev::ReturnValue getRgbFOV(std::vector<double>& horizontalFov, std::vector<double>& verticalFov) {
        horizontalFov.resize(1);
        verticalFov.resize(1);
        return self->getRgbFOV(horizontalFov[0], verticalFov[0]);
    }

    yarp::dev::ReturnValue getRgbMirroring(std::vector<bool>& mirror) {
        bool value = false;
        yarp::dev::ReturnValue ret = self->getRgbMirroring(value);
        mirror.assign(1, value);
        return ret;
    }
}

//////////////////////////////////////////////////////////////////////////
// Adding IDepthVisualParams (one-element vectors as output holders)
%extend yarp::dev::IDepthVisualParams {
    yarp::dev::ReturnValue getDepthResolution(std::vector<int>& width, std::vector<int>& height) {
        width.resize(1);
        height.resize(1);
        return self->getDepthResolution(width[0], height[0]);
    }

    yarp::dev::ReturnValue getDepthFOV(std::vector<double>& horizontalFov, std::vector<double>& verticalFov) {
        horizontalFov.resize(1);
        verticalFov.resize(1);
        return self->getDepthFOV(horizontalFov[0], verticalFov[0]);
    }

    yarp::dev::ReturnValue getDepthAccuracy(std::vector<double>& accuracy) {
        accuracy.resize(1);
        return self->getDepthAccuracy(accuracy[0]);
    }

    yarp::dev::ReturnValue getDepthClipPlanes(std::vector<double>& nearPlane, std::vector<double>& farPlane) {
        nearPlane.resize(1);
        farPlane.resize(1);
        return self->getDepthClipPlanes(nearPlane[0], farPlane[0]);
    }

    yarp::dev::ReturnValue getDepthMirroring(std::vector<bool>& mirror) {
        bool value = false;
        yarp::dev::ReturnValue ret = self->getDepthMirroring(value);
        mirror.assign(1, value);
        return ret;
    }
}

//////////////////////////////////////////////////////////////////////////
// Adding ILocalization2D

#if !defined(YARP_NO_MATH)
%extend yarp::dev::Nav2D::ILocalization2D {
    yarp::dev::Nav2D::LocalizationStatusEnum getLocalizationStatus() {
        yarp::dev::Nav2D::LocalizationStatusEnum status;
        self->getLocalizationStatus(status);
        return status;
    }

    std::vector<yarp::dev::Nav2D::Map2DLocation> getEstimatedPoses() {
        std::vector<yarp::dev::Nav2D::Map2DLocation> poses;
        self->getEstimatedPoses(poses);
        return poses;
    }

    yarp::dev::Nav2D::Map2DLocation getCurrentPosition() {
        yarp::dev::Nav2D::Map2DLocation loc;
        self->getCurrentPosition(loc);
        return loc;
    }
}

//////////////////////////////////////////////////////////////////////////
// Adding IMap2D
%extend yarp::dev::Nav2D::IMap2D {
    std::vector<std::string> get_map_names() {
        std::vector<std::string> map_names;
        self->get_map_names(map_names);
        return map_names;
    }

    std::vector<std::string> getObjectsList() {
        std::vector<std::string> objects;
        self->getObjectsList(objects);
        return objects;
    }

    std::vector<std::string> getLocationsList() {
        std::vector<std::string> locations;
        self->getLocationsList(locations);
        return locations;
    }

    std::vector<std::string> getAreasList() {
        std::vector<std::string> areas;
        self->getAreasList(areas);
        return areas;
    }

    std::vector<std::string> getPathsList() {
        std::vector<std::string> paths;
        self->getPathsList(paths);
        return paths;
    }
}

//////////////////////////////////////////////////////////////////////////
// Adding INavigation2D
%extend yarp::dev::Nav2D::INavigation2D {
    yarp::dev::Nav2D::NavigationStatusEnum getNavigationStatus() {
        yarp::dev::Nav2D::NavigationStatusEnum status;
        self->getNavigationStatus(status);
        return status;
    }

    yarp::dev::Nav2D::Map2DLocation getAbsoluteLocationOfCurrentTarget() {
        yarp::dev::Nav2D::Map2DLocation loc;
        self->getAbsoluteLocationOfCurrentTarget(loc);
        return loc;
    }

    yarp::dev::Nav2D::Map2DPath getAllNavigationWaypoints(yarp::dev::Nav2D::TrajectoryTypeEnum trajectory_type) {
        yarp::dev::Nav2D::Map2DPath waypoints;
        self->getAllNavigationWaypoints(trajectory_type, waypoints);
        return waypoints;
    }

    yarp::dev::Nav2D::Map2DLocation getCurrentNavigationWaypoint() {
        yarp::dev::Nav2D::Map2DLocation curr_waypoint;
        self->getCurrentNavigationWaypoint(curr_waypoint);
        return curr_waypoint;
    }
}
#endif
