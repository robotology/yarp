// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_RPCCLIENT_
#define _YARP2_RPCCLIENT_

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
class YARP_OS_API yarp::os::RpcClient : public UnbufferedContactable {
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
    virtual bool read(PortReader& reader, bool willReply = false);

    // documented in UnbufferedContactable
    virtual bool reply(PortWriter& writer);

    // documented in UnbufferedContactable
    virtual bool replyAndDrop(PortWriter& writer);

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

    virtual Type getType();

    virtual void promiseType(const Type& typ);

    virtual Property *acquireProperties(bool readOnly);
    virtual void releaseProperties(Property *prop);

   void setInputMode(bool expectInput);
   void setOutputMode(bool expectOutput);
   void setRpcMode(bool expectRpc);

private:
    // an RpcClient may be implemented with a regular port
    Port port;

    // forbid copy constructor and assignment operator by making them private
    // and not implementing them
    RpcClient(const RpcClient& alt);
    const RpcClient& operator = (const RpcClient& alt);
};

#endif
