// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_RPCSERVER_
#define _YARP2_RPCSERVER_

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
class YARP_OS_API yarp::os::RpcServer : public UnbufferedContactable {
public:
    using Contactable::open;


    /**
     * Constructor.
     */
    RpcServer();

    /**
     * Destructor.
     */
    virtual ~RpcServer();

    // documentation provided in Contactable
    virtual bool open(const ConstString& name);

    // documentation provided in Contactable
    virtual bool open(const Contact& contact, bool registerName = true);

    // documentation provided in Contactable
    virtual bool addOutput(const ConstString& name);

    // documentation provided in Contactable
    virtual bool addOutput(const ConstString& name, 
                           const ConstString& carrier);

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

    // documented in UnbufferedContactable
    virtual bool write(PortWriter& writer, 
                       PortWriter *callback = 0 /*NULL*/) const;

    // documented in UnbufferedContactable
    virtual bool write(PortWriter& writer, PortReader& reader,
                       PortWriter *callback = 0 /*NULL*/) const;

    // documented in UnbufferedContactable
    virtual bool read(PortReader& reader, bool willReply = true);

    // documented in UnbufferedContactable
    virtual bool reply(PortWriter& writer);

    // documented in UnbufferedContactable
    virtual bool replyAndDrop(PortWriter& writer);

    /**
     * Set an external reader for port data.
     * The reader will be able to reply by calling connection.getWriter().
     * @param reader the external reader to use
     */
    void setReader(PortReader& reader);

    /**
     * Set a creator for readers for port data.
     * Every port that input is received from will be automatically
     * given its own reader.  Handy if you care about the identity
     * of the receiver.
     *
     * @param creator the "factory" for creating PortReader object
     */
    void setReaderCreator(PortReaderCreator& creator);

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

    virtual Type getType();

    virtual void promiseType(const Type& typ);

    virtual Property *acquireProperties(bool readOnly);
    virtual void releaseProperties(Property *prop);

   void setInputMode(bool expectInput);
   void setOutputMode(bool expectOutput);
   void setRpcMode(bool expectRpc);

private:
    // an RpcServer may be implemented with a regular port
    // (this is not decided yet)
    Port port;

    // forbid copy constructor and assignment operator by making them private
    // and not implementing them
    RpcServer(const RpcServer& alt);
    const RpcServer& operator = (const RpcServer& alt);
};

#endif
