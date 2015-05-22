// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __RPC_MESSAGES_PARSER__
#define __RPC_MESSAGES_PARSER__

/*
* Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
* Author: Alberto Cardellino
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

// This file contains helper functions for the ControlBoardWrapper

#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/Wrapper.h>

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

/* Using yarp::dev::impl namespace for all helper class inside yarp::dev to reduce
 * name conflicts
 */

namespace yarp {
    namespace dev {
        class ControlBoardWrapper;
        namespace impl {
            class RPCMessagesParser;
            class SubDevice;
            class WrappedDevice;
        }
    }
}


#ifndef DOXYGEN_SHOULD_SKIP_THIS



/* the control command message type
* head is a Bottle which contains the specification of the message type
* body is a Vector which move the robot accordingly
*/
typedef yarp::os::PortablePair<yarp::os::Bottle, yarp::sig::Vector> CommandMessage;


/**
* Helper object for parsing RPC port messages
*/
class  yarp::dev::impl::RPCMessagesParser : public yarp::dev::DeviceResponder
{
protected:
    yarp::dev::ControlBoardWrapper      *ControlBoardWrapper_p;
    yarp::dev::IPidControl              *rpc_IPid;
    yarp::dev::IPositionControl         *rpc_IPosCtrl;
    yarp::dev::IPositionControl2        *rpc_IPosCtrl2;
    yarp::dev::IPositionDirect          *rpc_IPosDirect;
    yarp::dev::IVelocityControl         *rpc_IVelCtrl;
    yarp::dev::IVelocityControl2        *rpc_IVelCtrl2;
    yarp::dev::IEncodersTimed           *rpc_IEncTimed;
    yarp::dev::IMotorEncoders           *rpc_IMotEnc;
    yarp::dev::IAmplifierControl        *rcp_IAmp;
    yarp::dev::IControlLimits2          *rcp_Ilim2;
    yarp::dev::ITorqueControl           *rpc_ITorque;
    yarp::dev::IControlMode             *rpc_iCtrlMode;
    yarp::dev::IControlMode2            *rpc_iCtrlMode2;
    yarp::dev::IAxisInfo                *rpc_AxisInfo;
    yarp::dev::IRemoteCalibrator        *rpc_IRemoteCalibrator;
    yarp::dev::IControlCalibration2     *rpc_Icalib2;
    yarp::dev::IOpenLoopControl         *rpc_IOpenLoop;
    yarp::dev::IImpedanceControl        *rpc_IImpedance;
    yarp::dev::IInteractionMode         *rpc_IInteract;
    yarp::dev::IMotor                   *rpc_IMotor;
    yarp::sig::Vector                   tmpVect;
    yarp::os::Stamp                     lastRpcStamp;
    yarp::os::Semaphore                 mutex;
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
    void init(yarp::dev::ControlBoardWrapper *x);

    virtual bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response);

    void handleTorqueMsg(const yarp::os::Bottle& cmd,
        yarp::os::Bottle& response, bool *rec, bool *ok);

    void handleControlModeMsg(const yarp::os::Bottle& cmd,
        yarp::os::Bottle& response, bool *rec, bool *ok);

    void handleImpedanceMsg(const yarp::os::Bottle& cmd,
        yarp::os::Bottle& response, bool *rec, bool *ok);

    void handleInteractionModeMsg(const yarp::os::Bottle& cmd,
        yarp::os::Bottle& response, bool *rec, bool *ok);

    void handleOpenLoopMsg(const yarp::os::Bottle& cmd,
        yarp::os::Bottle& response, bool *rec, bool *ok);


    void handleProtocolVersionRequest(const yarp::os::Bottle& cmd,
         yarp::os::Bottle& response, bool *rec, bool *ok);

    void handleRemoteCalibratorMsg(const yarp::os::Bottle& cmd, yarp::os::Bottle& response, bool *rec, bool *ok);

    /**
    * Initialize the internal data.
    * @return true/false on success/failure
    */
    virtual bool initialize();
};




#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif  //__RPC_MESSAGES_PARSER__
