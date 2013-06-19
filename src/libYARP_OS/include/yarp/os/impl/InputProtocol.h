// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_INPUTPROTOCOL_
#define _YARP2_INPUTPROTOCOL_

#include <yarp/os/impl/Address.h>
#include <yarp/os/impl/Route.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/impl/InputStream.h>
#include <yarp/os/impl/OutputStream.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/Property.h>

namespace yarp {
    namespace os {
        namespace impl {
            class InputProtocol;
            class OutputProtocol;
        }
    }
}

/**
 * The input side of an active connection between two ports.
 */
class YARP_OS_impl_API yarp::os::impl::InputProtocol {
public:
    virtual ~InputProtocol() {}

    virtual bool open(const String& name) = 0;
    virtual void close() = 0;
    virtual void interrupt() = 0;

    virtual const Route& getRoute() = 0;

    virtual yarp::os::ConnectionReader& beginRead() = 0;
    virtual void endRead() = 0;

    // can metadata (port-level comms) be inserted?  if not,there is only data.
    virtual bool canEscape() = 0;

    virtual bool isPush() = 0;

    virtual bool checkStreams() = 0;

    virtual void resetStreams() = 0;

    // some connections are capable of ping-ponging
    virtual OutputProtocol& getOutput() = 0;
    virtual void suppressReply() = 0;
  
    // direct access
    virtual OutputStream& getOutputStream() = 0;
    virtual InputStream& getInputStream() = 0;
    virtual const Address& getRemoteAddress() = 0;

    virtual void setEnvelope(const String& str) = 0;

    virtual bool setTimeout(double timeout) = 0;

    virtual yarp::os::ConnectionReader& modifyIncomingData(yarp::os::ConnectionReader& reader) = 0;

    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader) = 0;

    virtual bool skipIncomingData(yarp::os::ConnectionReader& reader) = 0;

    virtual void setInputCarrierParams(const yarp::os::Property& params) = 0;
    
    virtual void getInputCarrierParams(yarp::os::Property& params) = 0;
};

#endif

