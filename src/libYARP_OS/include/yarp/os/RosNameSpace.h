/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

    virtual Contact getNameServerContact() const YARP_OVERRIDE;

    virtual Contact queryName(const ConstString& name) YARP_OVERRIDE;
    virtual Contact registerName(const ConstString& name) YARP_OVERRIDE;
    virtual Contact registerContact(const Contact& contact) YARP_OVERRIDE;
    virtual Contact unregisterName(const ConstString& name) YARP_OVERRIDE;
    virtual Contact unregisterContact(const Contact& contact) YARP_OVERRIDE;
    virtual Contact registerAdvanced(const Contact& contact,
                                     NameStore *store) YARP_OVERRIDE;
    virtual Contact unregisterAdvanced(const ConstString& name,
                                       NameStore *store) YARP_OVERRIDE;

    virtual bool setProperty(const ConstString& name,
                             const ConstString& key,
                             const Value& value) YARP_OVERRIDE;

    virtual Value *getProperty(const ConstString& name,
                               const ConstString& key) YARP_OVERRIDE;

    virtual bool connectPortToTopic(const Contact& src,
                                    const Contact& dest,
                                    ContactStyle style) YARP_OVERRIDE;
    virtual bool connectTopicToPort(const Contact& src,
                                    const Contact& dest,
                                    ContactStyle style) YARP_OVERRIDE;
    virtual bool disconnectPortFromTopic(const Contact& src,
                                         const Contact& dest,
                                         ContactStyle style) YARP_OVERRIDE;
    virtual bool disconnectTopicFromPort(const Contact& src,
                                         const Contact& dest,
                                         ContactStyle style) YARP_OVERRIDE;
    virtual bool connectPortToPortPersistently(const Contact& src,
                                               const Contact& dest,
                                               ContactStyle style) YARP_OVERRIDE;
    virtual bool disconnectPortToPortPersistently(const Contact& src,
                                                  const Contact& dest,
                                                  ContactStyle style) YARP_OVERRIDE;
    virtual bool connectTopic(Bottle& cmd,
                              bool srcIsTopic,
                              const Contact& src,
                              const Contact& dest,
                              ContactStyle style,
                              bool activeRegistration);

    virtual bool localOnly() const YARP_OVERRIDE;
    virtual bool usesCentralServer() const YARP_OVERRIDE;
    virtual bool serverAllocatesPortNumbers() const YARP_OVERRIDE;
    virtual bool connectionHasNameOfEndpoints() const YARP_OVERRIDE;

    virtual Contact detectNameServer(bool useDetectedServer,
                                     bool& scanNeeded,
                                     bool& serverUsed) YARP_OVERRIDE;
    virtual bool writeToNameServer(PortWriter& cmd,
                                   PortReader& reply,
                                   const ContactStyle& style) YARP_OVERRIDE;


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

    virtual void run() YARP_OVERRIDE;

private:
    Contact contact;
    Bottle pending;
    Semaphore mutex;
};

#endif // YARP_OS_ROSNAMESPACE_H
