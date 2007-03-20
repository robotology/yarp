// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __YARP2_DEVICEDRIVER__
#define __YARP2_DEVICEDRIVER__

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Property.h>
#include <yarp/os/IConfig.h>

namespace yarp {
    /**
     * An interface for the device drivers 
     */
    namespace dev {
        class DeviceDriver;
        class DeviceResponder;
    }
}

/**
 * \ingroup dev_class
 *
 * Interface implemented by all device drivers.
 */
class yarp::dev::DeviceDriver : public yarp::os::IConfig
{
public:
    /**
     * Destructor.
     */
    virtual ~DeviceDriver(){}

    /**
     * Open the DeviceDriver. 
     * @param config is a list of parameters for the device.
     * Which parameters are effective for your device can vary.
     * See \ref dev_examples "device invocation examples".
     * If there is no example for your device,
     * you can run the "yarpdev" program with the verbose flag
     * set to probe what parameters the device is checking.
     * If that fails too,
     * you'll need to read the source code (please nag one of the 
     * yarp developers to add documentation for your device).
     * @return true/false upon success/failure
     */
    virtual bool open(yarp::os::Searchable& config){ return true; }

    /**
     * Close the DeviceDriver.
     * @return true/false on success/failure.
     */
    virtual bool close(){ return true; }

};


/**
 * \ingroup dev_class
 *
 * A cheap and cheerful framework for human readable/writable forms of
 * messages to devices.  The main concern is to makes it easier to
 * document these messages.  Override DeviceResponder::respond() to
 * respond to new messages.  You don't need to use this class --
 * the network format of messages is defined independently of it.
 */
class yarp::dev::DeviceResponder : public yarp::os::PortReader,
            public yarp::os::TypedReaderCallback<yarp::os::Bottle>
{

private:
    yarp::os::Bottle examples;
    yarp::os::Bottle explains;
    yarp::os::Bottle details;

public:
    /**
     * Constructor
     */
    DeviceResponder();

    /**
     * Add information about a message that the respond() method
     * understands.
     * @param txt the message, in text form
     * @param explain an (optional) what the message means
     */
    void addUsage(const char *txt, const char *explain=0/*NULL*/);

    /**
     * Add information about a message that the respond() method
     * understands.
     * @param bot the message, in bottle form
     * @param explain an (optional) what the message means
     */
    void addUsage(const yarp::os::Bottle& bot, const char *explain=0/*NULL*/);

    /**
     * Respond to a message.  
     * @param command the message
     * @param reply the response
     * @return true if there was no critical failure
     */
    virtual bool respond(const yarp::os::Bottle& command, 
                         yarp::os::Bottle& reply);
   
    /**
     * Handler for reading messages from the network, and passing 
     * them on to the respond() method.
     * @param connection a network connection to a port
     * @return true if the message was read successfully
     */
    virtual bool read(yarp::os::ConnectionReader& connection);

    /**
     * Alternative handler for reading messages from the network, and passing 
     * them on to the respond() method.  There can be no replies made
     * if this handler is used.
     * @param v the message
     * @return true if the message was read successfully
     */
    virtual void onRead(yarp::os::Bottle& v) {
        yarp::os::Bottle reply;
        respond(v,reply);
    }

    /**
     * Regenerate usage information.
     */
    void makeUsage();

    /**
     * Attach this object to a source of messages.
     * @param source a BufferedPort or PortReaderBuffer that
     * receives data.
     */
    void attach(yarp::os::TypedReader<yarp::os::Bottle>& source) {
        source.useCallback(*this);
        source.setReplier(*this);
    }
};


#endif
