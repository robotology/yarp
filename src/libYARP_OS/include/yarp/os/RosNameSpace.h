/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_ROSNAMESPACE_H
#define YARP_OS_ROSNAMESPACE_H

#include <yarp/os/NameSpace.h>
#include <yarp/os/Network.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Semaphore.h>

#include <cstdio>

namespace yarp {
    namespace os {
        class RosNameSpace;
    }
}

class YARP_OS_API yarp::os::RosNameSpace : public NameSpace, public Thread {
public:
    RosNameSpace(const Contact& contact);

    virtual ~RosNameSpace();

    virtual Contact getNameServerContact() const override;

    virtual Contact queryName(const ConstString& name) override;
    virtual Contact registerName(const ConstString& name) override;
    virtual Contact registerContact(const Contact& contact) override;
    virtual Contact unregisterName(const ConstString& name) override;
    virtual Contact unregisterContact(const Contact& contact) override;
    virtual Contact registerAdvanced(const Contact& contact,
                                     NameStore *store) override;
    virtual Contact unregisterAdvanced(const ConstString& name,
                                       NameStore *store) override;

    virtual bool setProperty(const ConstString& name,
                             const ConstString& key,
                             const Value& value) override;

    virtual Value *getProperty(const ConstString& name,
                               const ConstString& key) override;

    virtual bool connectPortToTopic(const Contact& src,
                                    const Contact& dest,
                                    ContactStyle style) override;
    virtual bool connectTopicToPort(const Contact& src,
                                    const Contact& dest,
                                    ContactStyle style) override;
    virtual bool disconnectPortFromTopic(const Contact& src,
                                         const Contact& dest,
                                         ContactStyle style) override;
    virtual bool disconnectTopicFromPort(const Contact& src,
                                         const Contact& dest,
                                         ContactStyle style) override;
    virtual bool connectPortToPortPersistently(const Contact& src,
                                               const Contact& dest,
                                               ContactStyle style) override;
    virtual bool disconnectPortToPortPersistently(const Contact& src,
                                                  const Contact& dest,
                                                  ContactStyle style) override;
    virtual bool connectTopic(Bottle& cmd,
                              bool srcIsTopic,
                              const Contact& src,
                              const Contact& dest,
                              ContactStyle style,
                              bool activeRegistration);

    virtual bool localOnly() const override;
    virtual bool usesCentralServer() const override;
    virtual bool serverAllocatesPortNumbers() const override;
    virtual bool connectionHasNameOfEndpoints() const override;

    virtual Contact detectNameServer(bool useDetectedServer,
                                     bool& scanNeeded,
                                     bool& serverUsed) override;
    virtual bool writeToNameServer(PortWriter& cmd,
                                   PortReader& reply,
                                   const ContactStyle& style) override;


    /**
     * Possible ROS names are a subset of YARP names.
     * For nodes, in practice there isn't much restriction, except
     * ":" is definitely ruled out.  Since plenty of valid
     * YARP ports have a ":" in them, we need to quote this.
     */
    static ConstString toRosName(const ConstString& name);
    static ConstString fromRosName(const ConstString& name);
    static ConstString toRosNodeName(const ConstString& name);
    static ConstString fromRosNodeName(const ConstString& name);
    static Contact rosify(const Contact& contact);

    virtual void run() override;

private:
    Contact contact;
    Bottle pending;
    Semaphore mutex;
};

#endif // YARP_OS_ROSNAMESPACE_H
