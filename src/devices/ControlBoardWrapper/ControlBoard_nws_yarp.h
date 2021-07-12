/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARD_NWS_YARP_H
#define YARP_DEV_CONTROLBOARD_NWS_YARP_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/os/PeriodicThread.h>

#include <yarp/dev/IPidControl.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/IPositionDirect.h>
#include <yarp/dev/IVelocityControl.h>
#include <yarp/dev/IEncodersTimed.h>
#include <yarp/dev/IMotor.h>
#include <yarp/dev/IMotorEncoders.h>
#include <yarp/dev/IAmplifierControl.h>
#include <yarp/dev/IControlLimits.h>
#include <yarp/dev/IRemoteCalibrator.h>
#include <yarp/dev/IControlCalibration.h>
#include <yarp/dev/ITorqueControl.h>
#include <yarp/dev/IImpedanceControl.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/dev/IAxisInfo.h>
#include <yarp/dev/IInteractionMode.h>
#include <yarp/dev/IRemoteVariables.h>
#include <yarp/dev/IPWMControl.h>
#include <yarp/dev/ICurrentControl.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/IMotor.h>

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Stamp.h>
#include <yarp/sig/Vector.h>

#include <yarp/dev/impl/jointData.h>

#include <string>

#include "StreamingMessagesParser.h"
#include "RPCMessagesParser.h"


/**
 * @ingroup dev_impl_nws_yarp
 *
 * \brief `controlBoard_nws_yarp`: A controlBoard network wrapper server for YARP.
 *
 * \section controlBoard_nws_yarp_device_parameters Description of input parameters
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
 * | name           |      -         | string  | -              |   -           | Yes                         | full name of the port opened by the device, like /robotName/part/ | MUST start with a '/' character |
 * | period         |      -         | double  | s              |   0.02        | No                          | refresh period of the broadcasted values in s                     | optional, default 0.02s period|
 * | subdevice      |      -         | string  | -              |   -           | No                          | name of the subdevice to instantiate                              | when used, parameters for the subdevice must be provided as well |
 */

class ControlBoard_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread,
        public yarp::dev::WrapperSingle
{
private:
    std::string rootName;

    bool checkPortName(yarp::os::Searchable &params);

    yarp::os::BufferedPort<yarp::sig::Vector> outputPositionStatePort; // Port /state:o streaming out the encoder positions
    yarp::os::BufferedPort<CommandMessage> inputStreamingPort;         // Input streaming port for high frequency commands
    yarp::os::Port inputRPCPort;                                       // Input RPC port for set/get remote calls
    yarp::os::Port extendedOutputStatePort;                            // Port /stateExt:o streaming out the struct with the robot data

    yarp::os::PortWriterBuffer<yarp::dev::impl::jointData> extendedOutputState_buffer; // Buffer associated to the extendedOutputStatePort port
    yarp::os::PortReaderBuffer<yarp::os::Bottle> inputRPC_buffer;                      // Buffer associated to the inputRPCPort port

    RPCMessagesParser RPC_parser;             // Message parser associated to the inputRPCPort port
    StreamingMessagesParser streaming_parser; // Message parser associated to the inputStreamingPort port

    static constexpr double default_period = 0.02; // s
    double period {default_period};

    std::string partName; // to open ports and print more detailed debug messages
    yarp::sig::Vector times; // time for each joint
    yarp::os::Stamp time; // envelope to attach to the state port

    yarp::dev::DeviceDriver* subdevice_ptr{nullptr};
    bool subdevice_owned {true};
    size_t subdevice_joints {0};
    bool subdevice_ready = false;

    yarp::dev::IPidControl* iPidControl{nullptr};
    yarp::dev::IPositionControl* iPositionControl{nullptr};
    yarp::dev::IPositionDirect* iPositionDirect{nullptr};
    yarp::dev::IVelocityControl* iVelocityControl{nullptr};
    yarp::dev::IEncodersTimed* iEncodersTimed{nullptr};
    yarp::dev::IMotor* iMotor{nullptr};
    yarp::dev::IMotorEncoders* iMotorEncoders{nullptr};
    yarp::dev::IAmplifierControl* iAmplifierControl{nullptr};
    yarp::dev::IControlLimits* iControlLimits{nullptr};
    yarp::dev::IControlCalibration* iControlCalibration{nullptr};
    yarp::dev::ITorqueControl* iTorqueControl{nullptr};
    yarp::dev::IImpedanceControl* iImpedanceControl{nullptr};
    yarp::dev::IControlMode* iControlMode{nullptr};
    yarp::dev::IAxisInfo* iAxisInfo{nullptr};
    yarp::dev::IPreciselyTimed* iPreciselyTimed{nullptr};
    yarp::dev::IInteractionMode* iInteractionMode{nullptr};
    yarp::dev::IRemoteVariables* iRemoteVariables{nullptr};
    yarp::dev::IPWMControl* iPWMControl{nullptr};
    yarp::dev::ICurrentControl* iCurrentControl{nullptr};

    bool setDevice(yarp::dev::DeviceDriver* device, bool owned);
    bool openAndAttachSubDevice(yarp::os::Property& prop);

    void closeDevice();
    void closePorts();

public:
    ControlBoard_nws_yarp();
    ControlBoard_nws_yarp(const ControlBoard_nws_yarp&) = delete;
    ControlBoard_nws_yarp(ControlBoard_nws_yarp&&) = delete;
    ControlBoard_nws_yarp& operator=(const ControlBoard_nws_yarp&) = delete;
    ControlBoard_nws_yarp& operator=(ControlBoard_nws_yarp&&) = delete;
    ~ControlBoard_nws_yarp() override = default;

    // yarp::dev::DeviceDriver
    bool close() override;
    bool open(yarp::os::Searchable& prop) override;

    // yarp::dev::WrapperSingle
    bool attach(yarp::dev::PolyDriver* poly) override;
    bool detach() override;

    // yarp::os::PeriodicThread
    void run() override;
};


#endif // YARP_DEV_CONTROLBOARD_NWS_YARP_H
