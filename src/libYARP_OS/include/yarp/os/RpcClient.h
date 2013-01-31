// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_RPCCLIENT_
#define _YARP2_RPCCLIENT_

#include <yarp/os/Contactable.h>
#include <yarp/os/Port.h>

namespace yarp {
    namespace os {
        class RpcClient;
    }
}

/**
 * \ingroup comm_class
 *
 * A port that is specialized as an RPC client.  That is, it expects to
 * connect to a single server, and receive replies on the same connection.
 *
 */
class YARP_OS_API yarp::os::RpcClient : public Contactable {
public:
    using Contactable::open;


    /**
     * Constructor.
     */
    RpcClient() {
        // should configure port object to let it know it will be used
        // as an RPC client - TODO.
    }

    /**
     * Destructor.
     */
    virtual ~RpcClient() {
        port.close();
    }

    // documentation provided in Contactable
    virtual bool open(const char *name) {
        port.setInputMode(false);
        port.setOutputMode(true);
        port.setRpcMode(true);
        return port.open(name);
    }

    // documentation provided in Contactable
    virtual bool open(const Contact& contact, bool registerName = true) {
        return port.open(contact,registerName);
    }

    // documentation provided in Contactable
    virtual bool addOutput(const char *name) {
        return port.addOutput(name);
    }

    // documentation provided in Contactable
    virtual bool addOutput(const char *name, const char *carrier) {
        return port.addOutput(name,carrier);
    }

    // documentation provided in Contactable
    virtual bool addOutput(const Contact& contact){
        return port.addOutput(contact);
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
     * Write an object to the port, then expect one back.
     * @param writer any object that knows how to write itself to a
     * network connection - see for example Bottle
     * @param reader any object that knows how to read itself from a
     * network connection - see for example Bottle
     * @return true iff an object is successfully written and read
     */
    bool write(PortWriter& writer, PortReader& reader) const {
        return port.write(writer,reader);
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

    // documented in Contactable
    void setReader(PortReader& reader) {
        port.setReader(reader);
    }

private:
    // an RpcClient may be implemented with a regular port
    // (this is not decided yet)
    Port port;

    // forbid copy constructor by making it private
    RpcClient(const RpcClient& alt) : Contactable() {
    }

    // forbid assignment operator by making it private
    const RpcClient& operator = (const RpcClient& alt) {
        return *this;
    }
};

#endif
