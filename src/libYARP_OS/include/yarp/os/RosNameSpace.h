// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_ROSNAMESPACE_
#define _YARP2_ROSNAMESPACE_

#include <yarp/os/NameSpace.h>
#include <yarp/os/Network.h>
#include <yarp/os/Bottle.h>

#include <stdio.h>

namespace yarp {
    namespace os {
        class RosNameSpace;
    }
}

class yarp::os::RosNameSpace : public NameSpace {
public:
    RosNameSpace(const Contact& contact);

    virtual ~RosNameSpace();

    virtual Contact getNameServerContact() const;

    virtual Contact queryName(const ConstString& name);
    virtual Contact registerName(const ConstString& name);
    virtual Contact registerContact(const Contact& contact);
    virtual Contact unregisterName(const ConstString& name);
    virtual Contact unregisterContact(const Contact& contact);

    virtual bool setProperty(const ConstString& name, 
                             const ConstString& key, 
                             const Value& value);

    virtual Value *getProperty(const ConstString& name, 
                               const ConstString& key);

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
    virtual bool connectTopic(Bottle& cmd,
                              bool srcIsTopic,
                              const Contact& src,
                              const Contact& dest,
                              ContactStyle style,
                              bool activeRegistration);

    virtual bool localOnly() const;
    virtual bool usesCentralServer() const;
    virtual bool serverAllocatesPortNumbers() const;
    virtual bool connectionHasNameOfEndpoints() const;

    virtual Contact detectNameServer(bool useDetectedServer,
                                     bool& scanNeeded,
                                     bool& serverUsed);
    virtual bool writeToNameServer(PortWriter& cmd,
                                   PortReader& reply,
                                   const ContactStyle& style);


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

private:
    Contact contact;
};

#endif
