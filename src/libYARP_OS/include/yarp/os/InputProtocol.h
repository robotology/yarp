// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_INPUTPROTOCOL_
#define _YARP2_INPUTPROTOCOL_

#include <yarp/os/Route.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/InputStream.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Property.h>
#include <yarp/os/Connection.h>

namespace yarp {
    namespace os {
        class InputProtocol;
        class OutputProtocol;
        class Contactable;
    }
}

/**
 * The input side of an active connection between two ports.
 */
class YARP_OS_API yarp::os::InputProtocol {
public:
    virtual ~InputProtocol() {}

    virtual bool open(const ConstString& name) = 0;
    virtual void close() = 0;
    virtual void interrupt() = 0;

    virtual const Route& getRoute() = 0;

    virtual ConnectionReader& beginRead() = 0;
    virtual void endRead() = 0;

    virtual Connection& getConnection() = 0;
    virtual Connection& getReceiver() = 0;

    virtual bool checkStreams() = 0;

    // some connections are capable of ping-ponging
    virtual OutputProtocol& getOutput() = 0;
    virtual void suppressReply() = 0;
  
    // direct access
    virtual OutputStream& getOutputStream() = 0;
    virtual InputStream& getInputStream() = 0;

    virtual void setEnvelope(const ConstString& str) = 0;

    virtual bool setTimeout(double timeout) = 0;

    virtual void attachPort(Contactable *port) = 0;
};

#endif

