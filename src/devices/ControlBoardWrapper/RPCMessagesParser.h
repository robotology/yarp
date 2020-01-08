/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_RPCMESSAGESPARSER_H
#define YARP_DEV_CONTROLBOARDWRAPPER_RPCMESSAGESPARSER_H


// This file contains helper functions for the ControlBoardWrapper

#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/sig/Vector.h>

#include <mutex>
#include <string>
#include <vector>


#ifdef MSVC
    #pragma warning(disable:4355)
#endif

/*
 * To optimize memory allocation, for group of joints we can have one mem reserver for rpc port
 * and on e for streaming. The size could be numOfSubDevices*maxNumOfjointForSubdevice.
 * (we could also use the actual joint number for each subdevice using a for loop). TODO
 */

class ControlBoardWrapper;

/* the control command message type
* head is a Bottle which contains the specification of the message type
* body is a Vector which move the robot accordingly
*/
typedef yarp::os::PortablePair<yarp::os::Bottle, yarp::sig::Vector> CommandMessage;


/**
* Helper object for parsing RPC port messages
*/
class  RPCMessagesParser :
        public yarp::dev::DeviceResponder
{
protected:
    ControlBoardWrapper                 *ControlBoardWrapper_p;
    yarp::dev::IPidControl              *rpc_IPid;
    yarp::dev::IPositionControl         *rpc_IPosCtrl;
    yarp::dev::IPositionDirect          *rpc_IPosDirect;
    yarp::dev::IVelocityControl         *rpc_IVelCtrl;
    yarp::dev::IEncodersTimed           *rpc_IEncTimed;
    yarp::dev::IMotorEncoders           *rpc_IMotEnc;
    yarp::dev::IAmplifierControl        *rcp_IAmp;
    yarp::dev::IControlLimits           *rcp_Ilim;
    yarp::dev::ITorqueControl           *rpc_ITorque;
    yarp::dev::IControlMode             *rpc_iCtrlMode;
    yarp::dev::IAxisInfo                *rpc_AxisInfo;
    yarp::dev::IRemoteCalibrator        *rpc_IRemoteCalibrator;
    yarp::dev::IControlCalibration      *rpc_Icalib;
    yarp::dev::IImpedanceControl        *rpc_IImpedance;
    yarp::dev::IInteractionMode         *rpc_IInteract;
    yarp::dev::IMotor                   *rpc_IMotor;
    yarp::dev::IRemoteVariables         *rpc_IVar;
    yarp::dev::ICurrentControl          *rpc_ICurrent;
    yarp::dev::IPWMControl              *rpc_IPWM;
    yarp::sig::Vector                   tmpVect;
    yarp::os::Stamp                     lastRpcStamp;
    std::mutex                          mutex;
    int                                 controlledJoints;

public:
    /**
    * Constructor.
    */
    RPCMessagesParser();

    /**
    * Initialization.
    * @param x is the pointer to the instance of the object that uses the RPCMessagesParser.
    * This is required to recover the pointers to the interfaces that implement the responses
    * to the commands.
    */
    void init(ControlBoardWrapper *x);

    bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response) override;

    void handleTorqueMsg(const yarp::os::Bottle& cmd,
        yarp::os::Bottle& response, bool *rec, bool *ok);

    void handleControlModeMsg(const yarp::os::Bottle& cmd,
        yarp::os::Bottle& response, bool *rec, bool *ok);

    void handleImpedanceMsg(const yarp::os::Bottle& cmd,
        yarp::os::Bottle& response, bool *rec, bool *ok);

    void handleInteractionModeMsg(const yarp::os::Bottle& cmd,
        yarp::os::Bottle& response, bool *rec, bool *ok);

    void handleProtocolVersionRequest(const yarp::os::Bottle& cmd,
         yarp::os::Bottle& response, bool *rec, bool *ok);

    void handleRemoteCalibratorMsg(const yarp::os::Bottle& cmd, yarp::os::Bottle& response, bool *rec, bool *ok);

    void handleRemoteVariablesMsg(const yarp::os::Bottle& cmd, yarp::os::Bottle& response, bool *rec, bool *ok);

    void handleCurrentMsg(const yarp::os::Bottle& cmd, yarp::os::Bottle& response, bool *rec, bool *ok);

    void handlePWMMsg(const yarp::os::Bottle& cmd, yarp::os::Bottle& response, bool *rec, bool *ok);

    void handlePidMsg(const yarp::os::Bottle& cmd, yarp::os::Bottle& response, bool *rec, bool *ok);

    /**
    * Initialize the internal data.
    * @return true/false on success/failure
    */
    virtual bool initialize();
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_RPCMESSAGESPARSER_H
