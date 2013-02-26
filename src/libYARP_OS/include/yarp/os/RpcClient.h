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
    RpcClient();

    /**
     * Destructor.
     */
    virtual ~RpcClient();

    // documentation provided in Contactable
    virtual bool open(const char *name);

    // documentation provided in Contactable
    virtual bool open(const Contact& contact, bool registerName = true);

    // documentation provided in Contactable
    virtual bool addOutput(const char *name);

    // documentation provided in Contactable
    virtual bool addOutput(const char *name, const char *carrier);

    // documentation provided in Contactable
    virtual bool addOutput(const Contact& contact);

    // documentation provided in Contactable
    virtual void close();

    // documentation provided in Contactable
    virtual void interrupt();

    // documentation provided in Contactable
    virtual void resume();

    // documentation provided in Contactable
    virtual Contact where() const;

    // documentation provided in Contactable
    virtual ConstString getName() const;

    /**
     * Write an object to the port, then expect one back.
     * @param writer any object that knows how to write itself to a
     * network connection - see for example Bottle
     * @param reader any object that knows how to read itself from a
     * network connection - see for example Bottle
     * @return true iff an object is successfully written and read
     */
    bool write(PortWriter& writer, PortReader& reader) const;

    // documented in Contactable
    virtual bool setEnvelope(PortWriter& envelope);

    // documented in Contactable
    virtual bool getEnvelope(PortReader& envelope);

    // documented in Contactable
    virtual int getInputCount();

    // documented in Contactable
    virtual int getOutputCount();

    // documented in Contactable
    virtual void getReport(PortReport& reporter);

    // documented in Contactable
    virtual void setReporter(PortReport& reporter);

    // documented in Contactable
    virtual bool isWriting();

    // documented in Contactable
    void setReader(PortReader& reader);

private:
    // an RpcClient may be implemented with a regular port
    // (this is not decided yet)
    Port port;

    // forbid copy constructor and assignment operator by making them private
    // and not implementing them
    RpcClient(const RpcClient& alt);
    const RpcClient& operator = (const RpcClient& alt);
};

#endif
