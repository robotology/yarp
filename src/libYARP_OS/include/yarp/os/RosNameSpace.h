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

    virtual Contact queryName(const char *name);

    virtual Contact registerName(const char *name);

    virtual Contact registerContact(const Contact& contact);

    virtual Contact unregisterName(const char *name);

    virtual Contact unregisterContact(const Contact& contact);

    virtual bool setProperty(const char *name, const char *key,
                             const Value& value) {
        return false;
    }

    virtual Value *getProperty(const char *name, const char *key) {
        return 0/*NULL*/;
    }


    virtual bool connectPortToTopic(const Contact& src,
                                    const Contact& dest,
                                    ContactStyle style) {
        Bottle cmd;
        cmd.addString("registerPublisher");
        cmd.addString(toRosNodeName(src.getName()));
        cmd.addString(dest.getName());
        cmd.addString("*");
        cmd.addString(rosify(src).toString());

        return connectTopic(cmd,false,src,dest,style,false);
    }

    virtual bool connectTopicToPort(const Contact& src,
                                    const Contact& dest,
                                    ContactStyle style) {
        Bottle cmd;
        cmd.addString("registerSubscriber");
        cmd.addString(toRosNodeName(dest.getName()));
        cmd.addString(src.getName());
        cmd.addString("*");
        cmd.addString(rosify(dest).toString());

        return connectTopic(cmd,true,src,dest,style,true);
    }

    virtual bool disconnectPortFromTopic(const Contact& src,
                                         const Contact& dest,
                                         ContactStyle style) {
        Bottle cmd;
        cmd.addString("unregisterPublisher");
        cmd.addString(toRosNodeName(src.getName()));
        cmd.addString(dest.getName());
        cmd.addString(rosify(src).toString());
        return connectTopic(cmd,false,src,dest,style,false);
    }

    virtual bool disconnectTopicFromPort(const Contact& src,
                                         const Contact& dest,
                                         ContactStyle style) {
        Bottle cmd;
        cmd.addString("unregisterSubscriber");
        cmd.addString(toRosNodeName(dest.getName()));
        cmd.addString(src.getName());
        cmd.addString(rosify(dest).toString());
        return connectTopic(cmd,true,src,dest,style,false);
    }

    virtual bool connectPortToPortPersistently(const Contact& src,
                                               const Contact& dest,
                                               ContactStyle style) {
        return false;
    }

    virtual bool disconnectPortToPortPersistently(const Contact& src,
                                                  const Contact& dest,
                                                  ContactStyle style) {
        return false;
    }

    virtual bool connectTopic(Bottle& cmd,
                              bool srcIsTopic,
                              const Contact& src,
                              const Contact& dest,
                              ContactStyle style,
                              bool activeRegistration) {
        Bottle reply;
        Contact dynamicSrc = src;
        Contact dynamicDest = dest;
        if (style.carrier!="") {
            if (srcIsTopic) {
                dynamicDest = dynamicDest.addCarrier(style.carrier);
            } else {
                dynamicSrc = dynamicSrc.addCarrier(style.carrier);
            }
        }
        Contact base = getNameServerContact();
        bool ok = NetworkBase::write(base,
                                     cmd,
                                     reply);
        bool fail = (reply.check("faultCode",Value(0)).asInt()!=0)||!ok;
        if (fail) {
            if (!style.quiet) {
                fprintf(stderr,"Failure: name server did not accept connection to topic.\n");
                if (reply.check("faultString")) {
                    fprintf(stderr,"Cause: %s\n", reply.check("faultString",Value("")).asString().c_str());
                }
            }
        }
        if (!fail) {
            if (activeRegistration) {
                Bottle *lst = reply.get(2).asList();
                Bottle cmd2;
                if (lst!=NULL) {
                    cmd2.addString("publisherUpdate");
                    cmd2.addString("/yarp");
                    cmd2.addString(dynamicSrc.getName());
                    cmd2.addList() = *lst;
                    //printf("ACTIVE REGISTRATION: %s\n", cmd2.toString().c_str());
                    NetworkBase::write(dynamicDest,
                                       cmd2,
                                       reply,
                                       true);
                }
            }
        }
        return !fail;
    }

    virtual bool localOnly() const {
        return false;
    }

    virtual bool usesCentralServer() const {
        return true;
    }

    virtual bool serverAllocatesPortNumbers() const {
        return false;
    }

    virtual bool connectionHasNameOfEndpoints() const {
        return false;
    }


    /**
     * Possible ROS names are a subset of YARP names.
     * For nodes, in practice there isn't much restriction, except
     * ":" is definitely ruled out.  Since plenty of valid
     * YARP ports have a ":" in them, we need to quote this.
     */
    static ConstString toRosName(const ConstString& name);

    static ConstString fromRosName(const ConstString& name);


    static ConstString toRosNodeName(const ConstString& name) {
        return toRosName(name);
    }

    static ConstString fromRosNodeName(const ConstString& name) {
        return fromRosName(name);
    }

    static Contact rosify(const Contact& contact) {
        return Contact::bySocket("http",contact.getHost().c_str(),
                                 contact.getPort());
    }

    virtual Contact detectNameServer(bool useDetectedServer,
                                     bool& scanNeeded,
                                     bool& serverUsed);

    virtual bool writeToNameServer(PortWriter& cmd,
                                   PortReader& reply,
                                   const ContactStyle& style);

private:
    Contact contact;
};

#endif
