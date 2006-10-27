// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_CONTACTABLE_
#define _YARP2_CONTACTABLE_

#include <yarp/os/Contact.h>
#include <yarp/os/PortWriter.h>

/**
 * The main, catch-all namespace for YARP.
 */
namespace yarp {
    /**
     * An interface to the operating system, including Port based communication.
     */
    namespace os {
        class Contactable;
    }
}

/**
 * An abstract port.  Anything that can be expressed via a Contact.
 *
 */
class yarp::os::Contactable {

public:

    /**
     * Destructor.
     */
    virtual ~Contactable() {}

    /**
     * Start port operation, with automatically-chosen network parameters. 
     * The port is assigned an arbitrary name, and allocated
     * network resources,
     * by communicating with the YARP name server.
     * @return true iff the port started operation successfully and is now
     * visible on the YARP network
     */
    bool open() {
        return open("...");
    }

    /**
     * Start port operation, with a specific name, with
     * automatically-chosen network parameters.  The port is
     * registered with the given name, and allocated network
     * resources, by communicating with the YARP name server.  @return
     * true iff the port started operation successfully and is now
     * visible on the YARP network
     */
    virtual bool open(const char *name) = 0;

    /**
     * Start port operation with user-chosen network parameters.
     * Contact information is supplied by the user rather than
     * the name server.  If the Contact
     * information is incomplete, the name server is used
     * to complete it (set registerName to false if you don't
     * want name server help).
     * @return true iff the port started operation successfully and is now
     * visible on the YARP network
     */
    virtual bool open(const Contact& contact, bool registerName = true) = 0;

    /**
     * Add an output connection to the specified port.
     * @param name the name of the target port
     * @return true iff the connection is successfully created
     */
    virtual bool addOutput(const char *name) = 0;

    /**
     * Add an output connection to the specified port, using a specified
     * carrier
     * @param name the name of the target port
     * @param carrier the carrier (network protocol) to use, e.g. "tcp", "udp", "mcast", "text", ...
     * @return true iff the connection is successfully created
     */
    virtual bool addOutput(const char *name, const char *carrier) = 0;

    /**
     * Add an output connection to the specified port, using specified
     * network parameters.
     * @param contact information on how to reach the target
     * @return true iff the connection is successfully created
     */
    virtual bool addOutput(const Contact& contact) = 0;

    /**
     * Stop port activity.
     */
    virtual void close() = 0;

    /**
     * Returns information about how this port can be reached.
     * @return network parameters for this port
     */
    virtual Contact where() = 0;

    /**
     * Get name of port.
     * @return name of port
     */
    ConstString getName() {
        return where().getName();
    }


    /**
     * Every message sent by a port can have some information added
     * to it called an envelope (this could be a timestamp, for example).
     * You can set that envelope with this method.
     *
     * Currently, for proper operation, the envelope must serialize
     * correctly in text-mode (even if you do not explicitly use
     * text-mode connections).
     *
     * @param envelope some information to add to the next message sent
     *
     */
    virtual void setEnvelope(PortWriter& envelope) =0;

};

#endif
