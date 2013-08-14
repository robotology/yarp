// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PORTCOREUNIT_
#define _YARP2_PORTCOREUNIT_

#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/Name.h>

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
    PortCoreUnit(PortCore& owner, int index) : owner(owner), index(index) {
        doomed = false;
        hasMode = false;
        pupped = false;
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

    virtual void *send(yarp::os::PortWriter& writer,
                       yarp::os::PortReader *reader,
                       yarp::os::PortWriter *callback,
                       void *tracker,
                       const String& envelope,
                       bool waitAfter = true,
                       bool waitBefore = true,
                       bool *gotReply = NULL) {
        // do nothing
        return tracker;
    }

    virtual void *takeTracker() {
        return NULL;
    }

    virtual bool isBusy() {
        return false;
    }

    virtual bool interrupt() {
        return false;
    }

    void setMode() {
        Name name(getRoute().getCarrierName() + String("://test"));
        mode = name.getCarrierModifier("log",&hasMode);
    }

    int getIndex() {
        return index;
    }

    String getMode(bool *hasMode = NULL) {
        if (hasMode!=NULL) {
            *hasMode = this->hasMode;
        }
        return (this->hasMode)?mode:"";
    }

    bool isPupped() const {
        return pupped;
    }

    yarp::os::ConstString getPupString() const {
        return pupString;
    }

    void setPupped(bool flag, const yarp::os::ConstString& pupString) {
        pupped = flag;
        this->pupString = pupString;
    }

    virtual void setCarrierParams(const yarp::os::Property& params) { }
    virtual void getCarrierParams(yarp::os::Property& params) { }


protected:
    PortCore& getOwner() { return owner; }

private:
    PortCore& owner;
    bool doomed;
    String mode;
    bool hasMode;
    bool pupped;
    int index;
    yarp::os::ConstString pupString;
};

#endif

