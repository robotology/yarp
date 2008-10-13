// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_INPUTPROTOCOL_
#define _YARP2_INPUTPROTOCOL_

#include <yarp/Address.h>
#include <yarp/Route.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/InputStream.h>
#include <yarp/OutputStream.h>

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
class yarp::os::impl::InputProtocol {
public:
    virtual ~InputProtocol() {}

    // all may throw IOException

    virtual bool open(const String& name) = 0;
    virtual void close() = 0;
    virtual void interrupt() = 0;

    virtual const Route& getRoute() = 0;

    virtual yarp::os::ConnectionReader& beginRead() = 0;
    virtual void endRead() = 0;

    // can metadata (port-level comms) be inserted?  if not,there is only data.
    virtual bool canEscape() = 0;

    virtual bool checkStreams() = 0;

    virtual void resetStreams() = 0;

    // some connections are capable of ping-ponging
    virtual OutputProtocol& getOutput() = 0;
    virtual void suppressReply() = 0;
  
    // direct access
    virtual OutputStream& getOutputStream() = 0;
    virtual InputStream& getInputStream() = 0;
    virtual const Address& getRemoteAddress() = 0;
};

#endif

