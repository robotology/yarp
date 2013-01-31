// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_RPCSERVER_
#define _YARP2_RPCSERVER_

#include <yarp/os/Contactable.h>
#include <yarp/os/Port.h>


namespace yarp {
    namespace os {
        class RpcServer;
    }
}

/**
 * \ingroup comm_class
 *
 * A port that is specialized as an RPC server.  That is, it expects to
 * receive connections from multiple clients, and to reply to them via
 * those connections.
 *
 */
class YARP_OS_API yarp::os::RpcServer : public Contactable {
public:
    using Contactable::open;


    /**
     * Constructor.
     */
    RpcServer() {
        // should configure port object to let it know it will be used
        // as an RPC server - TODO.
    }

    /**
     * Destructor.
     */
    virtual ~RpcServer() {
        port.close();
    }

    // documentation provided in Contactable
    virtual bool open(const char *name) {
        port.setInputMode(true);
        port.setOutputMode(false);
        port.setRpcMode(true);
        return port.open(name);
    }

    // documentation provided in Contactable
    virtual bool open(const Contact& contact, bool registerName = true) {
        return port.open(contact,registerName);
    }

    // documentation provided in Contactable
    virtual bool addOutput(const char *name) {
        return false;
    }

    // documentation provided in Contactable
    virtual bool addOutput(const char *name, const char *carrier) {
        return false;
    }

    // documentation provided in Contactable
    virtual bool addOutput(const Contact& contact){
        return false;
    }

    // documentation provided in Contactable
    virtual void close() {
        port.close();
    }

    // documentation provided in Contactable
    virtual void interrupt() {
        port.interrupt();
    }

    // documentation provided in Contactable
    virtual void resume() {
        port.resume();
    }

    // documentation provided in Contactable
    virtual Contact where() const {
        return port.where();
    }

    // documentation provided in Contactable
    virtual ConstString getName() const {
        return where().getName();
    }

    /**
     * Read an object from the port.
     * @param reader any object that knows how to read itself from a
     * network connection - see for example Bottle
     * to it.
     * @param willReply you must set this to true if you intend to call reply().
     * For an RpcServer port, you must always call reply().  So this flag must
     * always be set to true.  It is here for consistency with the API
     * for yarp::os::Port
     * @return true iff the object is successfully read
     */
    bool read(PortReader& reader, bool willReply) {
        if (!willReply) {
            // this is an error for RpcServer
            return false;
        }
        return port.read(reader,true);
    }

   /**
     * Send an object as a reply to an object read from the port.
     * Only call this method if you set the willReply flag to
     * true when you called Port::read.
     * @param writer any object that knows how to write itself to a
     * network connection - see for example Bottle
     * @return true iff the object is successfully written
     */
    bool reply(PortWriter& writer) {
        return port.reply(writer);
    }

    /**
     * Set an external reader for port data.
     * The reader will be able to reply by calling connection.getWriter().
     * @param reader the external reader to use
     */
    void setReader(PortReader& reader) {
        port.setReader(reader);
    }

    /**
     * Set a creator for readers for port data.
     * Every port that input is received from will be automatically
     * given its own reader.  Handy if you care about the identity
     * of the receiver.
     *
     * @param creator the "factory" for creating PortReader object
     */
    void setReaderCreator(PortReaderCreator& creator) {
        port.setReaderCreator(creator);
    }

    // documented in Contactable
    virtual bool setEnvelope(PortWriter& envelope) {
        return port.setEnvelope(envelope);
    }

    // documented in Contactable
    virtual bool getEnvelope(PortReader& envelope) {
        return port.getEnvelope(envelope);
    }

    // documented in Contactable
    virtual int getInputCount() {
        return port.getInputCount();
    }

    // documented in Contactable
    virtual int getOutputCount() {
        return port.getOutputCount();
    }

    // documented in Contactable
    virtual void getReport(PortReport& reporter) {
        port.getReport(reporter);
    }

    // documented in Contactable
    virtual void setReporter(PortReport& reporter) {
        port.setReporter(reporter);
    }

    // documented in Contactable
    virtual bool isWriting() {
        return port.isWriting();
    }

private:
    // an RpcServer may be implemented with a regular port
    // (this is not decided yet)
    Port port;

    // forbid copy constructor by making it private
    RpcServer(const RpcServer& alt) : Contactable() {
    }

    // forbid assignment operator by making it private
    const RpcServer& operator = (const RpcServer& alt) {
        return *this;
    }
};

#endif
