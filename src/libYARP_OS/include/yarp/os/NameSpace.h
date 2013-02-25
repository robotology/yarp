// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_NAMESPACE_
#define _YARP2_NAMESPACE_

#include <yarp/os/Contact.h>
#include <yarp/os/Value.h>
#include <yarp/os/Network.h>

namespace yarp {
    namespace os {
        class NameSpace;
    }
}

class yarp::os::NameSpace {
public:
    virtual ~NameSpace() {}

    virtual bool checkNetwork();

    virtual bool checkNetwork(double timeout);

    virtual Contact getNameServerContact() const = 0;

    virtual ConstString getNameServerName() const {
        return getNameServerContact().getName();
    }

    virtual Contact queryName(const char *name) = 0;

    virtual Contact registerName(const char *name) = 0;

    virtual Contact registerContact(const Contact& contact) = 0;

    virtual Contact unregisterName(const char *name) = 0;

    virtual Contact unregisterContact(const Contact& contact) = 0;

    virtual bool setProperty(const char *name, const char *key,
                             const Value& value) = 0;

    virtual Value *getProperty(const char *name, const char *key) = 0;

    virtual bool connectPortToTopic(const Contact& src,
                                    const Contact& dest,
                                    ContactStyle style) = 0;

    virtual bool connectTopicToPort(const Contact& src,
                                    const Contact& dest,
                                    ContactStyle style) = 0;

    virtual bool disconnectPortFromTopic(const Contact& src,
                                         const Contact& dest,
                                         ContactStyle style) = 0;

    virtual bool disconnectTopicFromPort(const Contact& src,
                                         const Contact& dest,
                                         ContactStyle style) = 0;

    virtual bool connectPortToPortPersistently(const Contact& src,
                                               const Contact& dest,
                                               ContactStyle style) = 0;

    virtual bool disconnectPortToPortPersistently(const Contact& src,
                                                  const Contact& dest,
                                                  ContactStyle style) = 0;

    virtual bool localOnly() const = 0;

    virtual bool usesCentralServer() const = 0;

    virtual bool serverAllocatesPortNumbers() const = 0;

    virtual bool connectionHasNameOfEndpoints() const = 0;

    virtual Contact detectNameServer(bool useDetectedServer,
                                     bool& scanNeeded,
                                     bool& serverUsed) = 0;

    virtual bool writeToNameServer(PortWriter& cmd,
                                   PortReader& reply,
                                   const ContactStyle& style) = 0;

};

#endif

