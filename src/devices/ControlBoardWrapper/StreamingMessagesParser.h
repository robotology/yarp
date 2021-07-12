/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_STREAMINGMESSAGESPARSER_H
#define YARP_DEV_CONTROLBOARDWRAPPER_STREAMINGMESSAGESPARSER_H


// This file contains helper functions for the ControlBoardWrapper


#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/PolyDriver.h>

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


/* the control command message type
* head is a Bottle which contains the specification of the message type
* body is a Vector which move the robot accordingly
*/
typedef yarp::os::PortablePair<yarp::os::Bottle, yarp::sig::Vector> CommandMessage;

/**
* Callback implementation after buffered input.
*/
class StreamingMessagesParser : public yarp::os::TypedReaderCallback<CommandMessage>
{
protected:
    yarp::dev::IPositionControl* stream_IPosCtrl {nullptr};
    yarp::dev::IPositionDirect* stream_IPosDirect {nullptr};
    yarp::dev::IVelocityControl* stream_IVel {nullptr};
    yarp::dev::ITorqueControl* stream_ITorque {nullptr};
    yarp::dev::IPWMControl* stream_IPWM {nullptr};
    yarp::dev::ICurrentControl* stream_ICurrent {nullptr};
    int stream_nJoints {0};

public:
    /**
    * Constructor.
    */
    StreamingMessagesParser() = default;

    /**
     * Initialization.
     * @param x is the instance of the container class using the callback.
     */
    void init(yarp::dev::DeviceDriver* x);

    void reset();

    using yarp::os::TypedReaderCallback<CommandMessage>::onRead;
    /**
     * Callback function.
     * @param v is the Vector being received.
     */
    void onRead(CommandMessage& v) override;

    bool initialize();
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_STREAMINGMESSAGESPARSER_H
