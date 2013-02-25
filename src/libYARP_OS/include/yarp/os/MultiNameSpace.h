// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_MULTINAMESPACE_
#define _YARP2_MULTINAMESPACE_

#include <yarp/os/NameSpace.h>
#include <yarp/os/NameStore.h>

namespace yarp {
    namespace os {
        class MultiNameSpace;
    }
}

class yarp::os::MultiNameSpace : public NameSpace {
public:
    MultiNameSpace();

    virtual ~MultiNameSpace();

    bool setLocalMode(bool flag);

    bool activate(bool force = false);

    virtual Contact getNameServerContact() const;

    virtual Contact queryName(const char *name);

    virtual Contact registerName(const char *name);

    virtual Contact registerContact(const Contact& contact);

    virtual Contact unregisterName(const char *name);

    virtual Contact unregisterContact(const Contact& contact);

    virtual bool setProperty(const char *name, const char *key,
                             const Value& value);

    virtual Value *getProperty(const char *name, const char *key);

    virtual bool connectPortToTopic(const Contact& src,
                                    const Contact& dest,
                                    ContactStyle style);

    virtual bool connectTopicToPort(const Contact& src,
                                    const Contact& dest,
                                    ContactStyle style);

    virtual bool disconnectPortFromTopic(const Contact& src,
                                         const Contact& dest,
                                         ContactStyle style);

    virtual bool disconnectTopicFromPort(const Contact& src,
                                         const Contact& dest,
                                         ContactStyle style);

    virtual bool connectPortToPortPersistently(const Contact& src,
                                               const Contact& dest,
                                               ContactStyle style);

    virtual bool disconnectPortToPortPersistently(const Contact& src,
                                                  const Contact& dest,
                                                  ContactStyle style);

    virtual bool localOnly() const;

    virtual bool usesCentralServer() const;

    virtual bool serverAllocatesPortNumbers() const;

    virtual bool connectionHasNameOfEndpoints() const;

    virtual void queryBypass(NameStore *store) {
        altStore = store;
    }

    virtual NameStore *getQueryBypass() {
        return altStore;
    }

    virtual Contact detectNameServer(bool useDetectedServer,
                                     bool& scanNeeded,
                                     bool& serverUsed);

    virtual bool writeToNameServer(PortWriter& cmd,
                                   PortReader& reply,
                                   const ContactStyle& style);

private:
    void *system_resource;
    NameStore *altStore;
};

#endif
