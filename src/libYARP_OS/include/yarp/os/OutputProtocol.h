// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_OUTPUTPROTOCOL_
#define _YARP2_OUTPUTPROTOCOL_

#include <yarp/os/Route.h>
#include <yarp/os/InputStream.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/SizedWriter.h>
#include <yarp/os/Property.h>
#include <yarp/os/Connection.h>

namespace yarp {
    namespace os {
        class OutputProtocol;
        class InputProtocol;
        class Contactable;
    }
}

/**
 * The output side of an active connection between two ports.
 */
class YARP_OS_API yarp::os::OutputProtocol {
public:
    /**
     * Destructor.
     */
    virtual ~OutputProtocol() {}

    /**
     * Start negotiating a carrier, using the given route (this
     * should generally match the name of the sending port, the
     * receiving port, and the desired carrier.
     */
    virtual bool open(const Route& route) = 0;

    /**
     * Relabel the route after the fact (e.g. if the direction
     * of initiative switches and the route essentially 
     * reverses)
     */
    virtual void rename(const Route& route) = 0;

    /**
     *
     * Negotiate an end to operations.
     *
     */
    virtual void close() = 0;

    virtual const Route& getRoute() = 0;

    /**
     *
     * Check if the connection is valid and can be used.
     *
     */
    virtual bool isOk() = 0;

    /**
     *
     * Get the connection whose protocol operations we are managing.
     *
     */
    virtual Connection& getConnection() = 0;

    /**
     *
     * It is possible to chain a basic connection with a modifier.
     * If there is a receiver modifier, this call returns it, otherwise it
     * will return a connection for which yarp::os::Connection::isValid()
     * returns false.
     *
     */
    virtual Connection& getReceiver() = 0;

    /**
     *
     * It is possible to chain a basic connection with a modifier.
     * If there is a sender modifier, this call returns it, otherwise it
     * will return a connection for which yarp::os::Connection::isValid()
     * returns false.
     *
     */
    virtual Connection& getSender() = 0;

    /**
     *
     * Write a message on the connection.  If a reply is expected,
     * we wait for one.  If an acknowledgement is expected, we wait for
     * one.  Replies and acknowledgements are similar, except:
     * replies are at the client payload level, acknowledgements
     * are low-level protocol elements that the client doesn't need
     * to know about.
     *
     */
    virtual bool write(SizedWriter& writer) = 0;

    virtual void interrupt() = 0;

    
    /**
     *
     * Get an interface for doing read operations on the connection.
     * Not all types of connections will support this, check
     * yarp::os::InputProtocol::isOk() on the result.
     *
     */
    virtual InputProtocol& getInput() = 0;

    /**
     *
     * Access the output stream associated with the connection.
     *
     */
    virtual OutputStream& getOutputStream() = 0;

    /**
     *
     * Access the input stream associated with the connection.
     *
     */
    virtual InputStream& getInputStream() = 0;

    /**
     *
     * Set the timeout to be used for network operations.
     *
     */
    virtual bool setTimeout(double timeout) = 0;

    /**
     *
     * Set the port to be associated with the connection.
     *
     */
    virtual void attachPort(Contactable *port) = 0;
    
    /**
     *
     * Notify connection that we intend to write to it.
     * This only needs to get called once. There is no endWrite().
     *
     */
    virtual void beginWrite() = 0;
};

#endif
