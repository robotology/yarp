/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_RPCMESSAGESPARSER_H
#define YARP_DEV_CONTROLBOARDWRAPPER_RPCMESSAGESPARSER_H


// This file contains helper functions for the ControlBoardWrapper

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/PolyDriver.h>

#include <mutex>
#include <string>
#include <vector>


#ifdef MSVC
#    pragma warning(disable : 4355)
#endif

/*
 * To optimize memory allocation, for group of joints we can have one mem reserver for rpc port
 * and on e for streaming. The size could be numOfSubDevices*maxNumOfjointForSubdevice.
 * (we could also use the actual joint number for each subdevice using a for loop). TODO
 */

class ControlBoardWrapperCommon;

/* the control command message type
* head is a Bottle which contains the specification of the message type
* body is a Vector which move the robot accordingly
*/
typedef yarp::os::PortablePair<yarp::os::Bottle, yarp::sig::Vector> CommandMessage;


/**
* Helper object for parsing RPC port messages
*/
class RPCMessagesParser :
        public yarp::dev::DeviceResponder
{
protected:
    yarp::dev::IPidControl* rpc_IPid {nullptr};
    yarp::dev::IPositionControl* rpc_IPosCtrl {nullptr};
    yarp::dev::IPositionDirect* rpc_IPosDirect {nullptr};
    yarp::dev::IVelocityControl* rpc_IVelCtrl {nullptr};
    yarp::dev::IEncodersTimed* rpc_IEncTimed {nullptr};
    yarp::dev::IMotorEncoders* rpc_IMotEnc {nullptr};
    yarp::dev::IAmplifierControl* rcp_IAmp {nullptr};
    yarp::dev::IControlLimits* rcp_Ilim {nullptr};
    yarp::dev::ITorqueControl* rpc_ITorque {nullptr};
    yarp::dev::IControlMode* rpc_iCtrlMode {nullptr};
    yarp::dev::IAxisInfo* rpc_AxisInfo {nullptr};
    yarp::dev::IRemoteCalibrator* rpc_IRemoteCalibrator {nullptr};
    yarp::dev::IControlCalibration* rpc_Icalib {nullptr};
    yarp::dev::IImpedanceControl* rpc_IImpedance {nullptr};
    yarp::dev::IInteractionMode* rpc_IInteract {nullptr};
    yarp::dev::IMotor* rpc_IMotor {nullptr};
    yarp::dev::IRemoteVariables* rpc_IVar {nullptr};
    yarp::dev::ICurrentControl* rpc_ICurrent {nullptr};
    yarp::dev::IPWMControl* rpc_IPWM {nullptr};
    yarp::sig::Vector tmpVect;
    yarp::os::Stamp lastRpcStamp;
    std::mutex mutex;
    size_t controlledJoints {0};

public:
    /**
    * Constructor.
    */
    RPCMessagesParser() = default;

    /**
    * Initialization.
    * @param x is the pointer to the instance of the object that uses the RPCMessagesParser.
    * This is required to recover the pointers to the interfaces that implement the responses
    * to the commands.
    */
    void init(yarp::dev::DeviceDriver* x);
    void reset();

    bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response) override;

    void handleTorqueMsg(const yarp::os::Bottle& cmd,
                         yarp::os::Bottle& response,
                         bool* rec,
                         bool* ok);

    void handleControlModeMsg(const yarp::os::Bottle& cmd,
                              yarp::os::Bottle& response,
                              bool* rec,
                              bool* ok);

    void handleImpedanceMsg(const yarp::os::Bottle& cmd,
                            yarp::os::Bottle& response,
                            bool* rec,
                            bool* ok);

    void handleInteractionModeMsg(const yarp::os::Bottle& cmd,
                                  yarp::os::Bottle& response,
                                  bool* rec,
                                  bool* ok);

    void handleProtocolVersionRequest(const yarp::os::Bottle& cmd,
                                      yarp::os::Bottle& response,
                                      bool* rec,
                                      bool* ok);

    void handleRemoteCalibratorMsg(const yarp::os::Bottle& cmd, yarp::os::Bottle& response, bool* rec, bool* ok);

    void handleRemoteVariablesMsg(const yarp::os::Bottle& cmd, yarp::os::Bottle& response, bool* rec, bool* ok);

    void handleCurrentMsg(const yarp::os::Bottle& cmd, yarp::os::Bottle& response, bool* rec, bool* ok);

    void handlePWMMsg(const yarp::os::Bottle& cmd, yarp::os::Bottle& response, bool* rec, bool* ok);

    void handlePidMsg(const yarp::os::Bottle& cmd, yarp::os::Bottle& response, bool* rec, bool* ok);

    /**
    * Initialize the internal data.
    * @return true/false on success/failure
    */
    virtual bool initialize();
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_RPCMESSAGESPARSER_H
