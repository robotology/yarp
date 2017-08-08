/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Alberto Cardellino
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_STREAMINGMESSAGESPARSER_H
#define YARP_DEV_CONTROLBOARDWRAPPER_STREAMINGMESSAGESPARSER_H


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
            class StreamingMessagesParser;
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
* Callback implementation after buffered input.
*/
class  yarp::dev::impl::StreamingMessagesParser : public yarp::os::TypedReaderCallback<CommandMessage> {
protected:
    yarp::dev::IPositionControl     *stream_IPosCtrl;
    yarp::dev::IPositionControl2    *stream_IPosCtrl2;
    yarp::dev::IPositionDirect      *stream_IPosDirect;
    yarp::dev::IVelocityControl     *stream_IVel;
    yarp::dev::IVelocityControl2    *stream_IVel2;
    yarp::dev::ITorqueControl       *stream_ITorque;
    yarp::dev::IPWMControl          *stream_IPWM;
    yarp::dev::ICurrentControl      *stream_ICurrent;
    int                              stream_nJoints;

public:
    /**
    * Constructor.
    */
    StreamingMessagesParser();

    /**
    * Initialization.
    * @param x is the instance of the container class using the callback.
    */
    void init(yarp::dev::ControlBoardWrapper *x);

    using yarp::os::TypedReaderCallback<CommandMessage>::onRead;
    /**
    * Callback function.
    * @param v is the Vector being received.
    */
    virtual void onRead(CommandMessage& v) override;

    bool initialize();
};




#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif // YARP_DEV_CONTROLBOARDWRAPPER_STREAMINGMESSAGESPARSER_H
