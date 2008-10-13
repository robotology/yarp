// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_PORTMANAGER_
#define _YARP2_PORTMANAGER_

#include <yarp/String.h>
#include <yarp/OutputStream.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/PortReport.h>
#include <yarp/Logger.h>

#include <ace/OS_NS_stdio.h>

namespace yarp {
    namespace os {
        namespace impl {
            class PortManager;
        }
    }
}

/**
 * Specification of minimal operations a port must support.
 */
class yarp::os::impl::PortManager {
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

    virtual void addOutput(const String& dest, void *id, OutputStream *os) {
        ACE_OS::printf("ADDOUTPUT\n");
        ACE_DEBUG((LM_ERROR,"PortManager for [%s] asked to addOutput [%s]\n",
                   getName().c_str(),
                   dest.c_str()));
    }

    virtual void removeInput(const String& src, void *id, OutputStream *os) {
        ACE_DEBUG((LM_ERROR,"PortManager for [%s] asked to removeInput [%s]\n",
                   getName().c_str(),
                   src.c_str()));
    }

    virtual void removeOutput(const String& dest, void *id, OutputStream *os) {
        ACE_DEBUG((LM_ERROR,"PortManager for [%s] asked to removeOutput [%s]\n",
                   getName().c_str(),
                   dest.c_str()));
    }
  
    virtual void describe(void *id, OutputStream *os) {
        ACE_DEBUG((LM_ERROR,"PortManager for [%s] asked to describe itself\n",
                   getName().c_str()));
    }

    virtual bool readBlock(ConnectionReader& reader, void *id, OutputStream *os) {
        ACE_DEBUG((LM_ERROR,"PortManager for [%s] asked to deal with data\n",
                   getName().c_str()));
        return false;
    }

    virtual bool adminBlock(ConnectionReader& reader, void *id, OutputStream *os) {
        ACE_DEBUG((LM_ERROR,"PortManager for [%s] asked to deal with admin\n",
                   getName().c_str()));
        return false;
    }

    virtual String getName() {
        return String(name);
    }

    virtual void setEnvelope(const String& envelope) = 0;

    virtual void report(const yarp::os::PortInfo& info) = 0;

protected:
    bool hasOutput() {
        return os!=NULL;
    }

    OutputStream& getOutputStream() {
        YARP_ASSERT(os!=NULL);
        return *os;
    }

private:
    OutputStream *os;
    String name;
};

#endif

