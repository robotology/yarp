// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_PORTCOREUNIT_
#define _YARP2_PORTCOREUNIT_

#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/impl/Name.h>

namespace yarp {
    namespace os {
        namespace impl {
            class PortCoreUnit;
        }
    }
}

/**
 * This is a helper for the core port class, PortCore.
 * It manages a single threaded resource related to a single
 * connection.
 */

class yarp::os::impl::PortCoreUnit : public ThreadImpl {
public:
    PortCoreUnit(PortCore& owner) : owner(owner) {
        doomed = false;
        hasMode = false;
    }

    virtual ~PortCoreUnit() {
    }

    virtual bool isInput() {
        return false;
    }

    virtual bool isOutput() {
        return false;
    }

    virtual bool isFinished() {
        return false;
    }

    virtual Route getRoute() {
        return Route("null","null","null");
    }

    bool isDoomed() {
        return doomed;
    }

    void setDoomed(bool flag = true) {
        doomed = flag;
    }

    virtual void *send(Writable& writer, 
                       Readable *reader,
                       Writable *callback,
                       void *tracker,
                       const String& envelope,                       
                       bool waitAfter = true,
                       bool waitBefore = true) {
        // do nothing
        return tracker;
    }

    virtual void *takeTracker() {
        return NULL;
    }

    virtual bool isBusy() {
        return false;
    }

    void setMode() {
        Name name(getRoute().getCarrierName() + String("://test"));
        mode = name.getCarrierModifier("log",&hasMode);
    }

    String getMode(bool *hasMode = NULL) {
        if (hasMode!=NULL) {
            *hasMode = this->hasMode;
        }
        return (this->hasMode)?mode:"";
    }

protected:
    PortCore& getOwner() { return owner; }

private:
    PortCore& owner;
    bool doomed;
    String mode;
    bool hasMode;
};

#endif

