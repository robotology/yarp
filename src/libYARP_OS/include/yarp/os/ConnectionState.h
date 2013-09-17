// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_CONNECTIONSTATE_
#define _YARP2_CONNECTIONSTATE_

#include <yarp/os/Route.h>
#include <yarp/os/TwoWayStream.h>
#include <yarp/os/Log.h>
#include <yarp/os/Contactable.h>

namespace yarp {
    namespace os {
        class ConnectionState;
        class Connection;
    }
}


class YARP_OS_API yarp::os::ConnectionState {
public:

    /**
     * Destructor.
     */
    virtual ~ConnectionState() {
    }

    virtual const Route& getRoute() = 0;
    virtual void setRoute(const Route& route) = 0;
    virtual OutputStream& os() = 0;
    virtual InputStream& is() = 0;
    virtual Connection& getConnection() = 0;
    virtual void setRemainingLength(int len) = 0;
    virtual Log& getLog() = 0;
    virtual ConstString getSenderSpecifier() = 0;
    virtual TwoWayStream& getStreams() = 0;
    virtual void takeStreams(TwoWayStream *streams) = 0;
    virtual TwoWayStream *giveStreams() = 0;
    virtual void setReference(yarp::os::Portable *ref) = 0;
    virtual bool checkStreams() = 0;
    virtual Contactable *getContactable() = 0;
};

#endif
