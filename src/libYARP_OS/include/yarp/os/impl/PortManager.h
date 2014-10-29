// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PORTMANAGER_
#define _YARP2_PORTMANAGER_

#include <yarp/os/impl/String.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/PortReport.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Route.h>

#include <yarp/os/impl/PlatformStdio.h>

namespace yarp {
    namespace os {
        namespace impl {
            class PortManager;
            class PortCoreUnit;
        }
    }
}

/**
 * Specification of minimal operations a port must support.
 */
class YARP_OS_impl_API yarp::os::impl::PortManager {
public:
    PortManager() {
        os = NULL;
        name = "null";
    }

    virtual ~PortManager() {
    }

    void setName(const String& name) {
        this->name = name;
    }

    virtual bool addOutput(const String& dest, void *id, 
                           yarp::os::OutputStream *os,
                           bool onlyIfNeeded = false) {
        YARP_SPRINTF2(Logger::get(),
                      error,
                      "PortManager for [%s] asked to addOutput [%s]\n",
                      getName().c_str(),
                      dest.c_str());
        return false;
    }

    virtual void removeInput(const String& src, void *id, 
                             yarp::os::OutputStream *os) {
        YARP_SPRINTF2(Logger::get(),
                      error,
                      "PortManager for [%s] asked to removeInput [%s]\n",
                      getName().c_str(),
                      src.c_str());
    }

    virtual void removeOutput(const String& dest, void *id, 
                              yarp::os::OutputStream *os) {
        YARP_SPRINTF2(Logger::get(),
                      error,
                      "PortManager for [%s] asked to removeOutput [%s]\n",
                      getName().c_str(),
                      dest.c_str());
    }

    virtual bool removeIO(const Route& route, bool synch = false) {
        return false;
    }

    virtual void describe(void *id, yarp::os::OutputStream *os) {
        YARP_SPRINTF1(Logger::get(),
                      error,
                      "PortManager for [%s] asked to describe itself\n",
                      getName().c_str());
    }

    virtual bool readBlock(ConnectionReader& reader, void *id, 
                           yarp::os::OutputStream *os) {
        YARP_SPRINTF1(Logger::get(),
                      error,
                      "PortManager for [%s] asked to deal with data\n",
                      getName().c_str());
        return false;
    }

    virtual bool adminBlock(ConnectionReader& reader, void *id, 
                            yarp::os::OutputStream *os) {
        YARP_SPRINTF1(Logger::get(),
                      error,
                      "PortManager for [%s] asked to deal with admin\n",
                      getName().c_str());
        return false;
    }

    virtual String getName() {
        return String(name);
    }

    virtual void setEnvelope(const String& envelope) = 0;

    virtual void report(const yarp::os::PortInfo& info) = 0;

    virtual void reportUnit(PortCoreUnit *unit, bool active) = 0;

protected:
    bool hasOutput() {
        return os!=NULL;
    }

    yarp::os::OutputStream& getOutputStream() {
        yAssert(os!=NULL);
        return *os;
    }

private:
    yarp::os::OutputStream *os;
    String name;
};

#endif

