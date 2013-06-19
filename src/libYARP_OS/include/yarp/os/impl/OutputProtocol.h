// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_OUTPUTPROTOCOL_
#define _YARP2_OUTPUTPROTOCOL_

#include <yarp/os/impl/Route.h>
#include <yarp/os/impl/InputStream.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/impl/SizedWriter.h>
#include <yarp/os/Property.h>

namespace yarp {
    namespace os {
        namespace impl {
            class OutputProtocol;
            class InputProtocol;
        }
    }
}

/**
 * The output side of an active connection between two ports.
 */
class YARP_OS_impl_API yarp::os::impl::OutputProtocol {
public:
    virtual ~OutputProtocol() {}

    virtual bool open(const Route& route) = 0;
    virtual void rename(const Route& route) = 0;
    virtual void close() = 0;

    virtual const Route& getRoute() = 0;
    virtual bool isActive() = 0;
    virtual bool isOk() = 0;
    virtual bool isTextMode() = 0;
    virtual bool isConnectionless() = 0;
    virtual bool isBroadcast() = 0;
    virtual bool canEscape() = 0;
    virtual bool supportReply() = 0;
    virtual bool isLocal() = 0;
    virtual bool isPush() = 0;
    virtual void prepareDisconnect() = 0;

    virtual bool checkStreams() = 0;

    virtual bool write(SizedWriter& writer) = 0;

    virtual void interrupt() = 0;

    // some connections are capable of ping-ponging
    virtual InputProtocol& getInput() = 0;

    // direct access
    virtual OutputStream& getOutputStream() = 0;
    virtual InputStream& getInputStream() = 0;

    virtual bool setTimeout(double timeout) = 0;

    virtual void setOutputCarrierParams(const yarp::os::Property& params) = 0;
    
    virtual void getOutputCarrierParams(yarp::os::Property& params) = 0;

};

#endif
