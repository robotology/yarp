/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_YARPNAMESPACE_H
#define YARP_OS_YARPNAMESPACE_H

#include <yarp/os/NameSpace.h>

#include <stdio.h>

namespace yarp {
    namespace os {
        class YarpNameSpace;
        class YarpDummyNameSpace;
    }
}

class YARP_OS_API yarp::os::YarpNameSpace : public NameSpace {
public:
    YarpNameSpace(const Contact& contact);

    virtual ~YarpNameSpace();

    virtual Contact getNameServerContact() const {
        return contact;
    }
    
    virtual Contact queryName(const ConstString& name);

    virtual Contact registerName(const ConstString& name);

    virtual Contact registerContact(const Contact& contact);

    virtual Contact unregisterName(const ConstString& name);

    virtual Contact unregisterContact(const Contact& contact);

    virtual bool setProperty(const ConstString& name, const ConstString& key, 
                             const Value& value);

    virtual Value *getProperty(const ConstString& name, const ConstString& key);

    virtual bool connectPortToTopic(const Contact& src, 
                                    const Contact& dest,
                                    ContactStyle style) {
        return connectTopic("subscribe",false,true,src,dest,style);
    }

    virtual bool connectTopicToPort(const Contact& src, 
                                    const Contact& dest,
                                    ContactStyle style) {
        return connectTopic("subscribe",true,false,src,dest,style);
    }

    virtual bool disconnectPortFromTopic(const Contact& src, 
                                         const Contact& dest,
                                         ContactStyle style) {
        return connectTopic("unsubscribe",false,true,src,dest,style);
    }

    virtual bool disconnectTopicFromPort(const Contact& src, 
                                         const Contact& dest,
                                         ContactStyle style) {
        return connectTopic("unsubscribe",true,false,src,dest,style);
    }

    virtual bool connectPortToPortPersistently(const Contact& src, 
                                               const Contact& dest,
                                               ContactStyle style) {
        return connectTopic("subscribe",false,false,src,dest,style);
    }

    virtual bool disconnectPortToPortPersistently(const Contact& src, 
                                                  const Contact& dest,
                                                  ContactStyle style) {
        return connectTopic("unsubscribe",false,false,src,dest,style);
    }

    virtual bool connectTopic(const ConstString& dir,
                              bool srcIsTopic,
                              bool destIsTopic,
                              const Contact& src, 
                              const Contact& dest,
                              ContactStyle style) {
        Contact dynamicSrc = src;
        Contact dynamicDest = dest;
        Bottle cmd, reply;
        cmd.addString(dir.c_str());
        if (style.carrier!="") {
            if (!destIsTopic) {
                dynamicDest.setCarrier(style.carrier);
            } else {
                dynamicSrc.setCarrier(style.carrier);
            }
        }
        cmd.addString(dynamicSrc.toString().c_str());
        cmd.addString(dynamicDest.toString().c_str());
        if (style.persistent) {
            switch (style.persistenceType) {
            case ContactStyle::END_WITH_FROM_PORT:
                cmd.addString("from");
                break;
            case ContactStyle::END_WITH_TO_PORT:
                cmd.addString("to");
                break;
            default:
                break;
            }
        }
        bool ok = false;
        if (!NetworkBase::getQueryBypass()) {
            ok = NetworkBase::write(getNameServerContact(),
                                    cmd,
                                    reply);
        } else {
            ContactStyle style;
            ok = NetworkBase::writeToNameServer(cmd,
                                                reply,
                                                style);
        }
        bool fail = (reply.get(0).toString()=="fail")||!ok;
        if (fail) {
            if (!style.quiet) {
                fprintf(stderr,"Failure: name server did not accept connection to topic.\n");
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
        return true;
    }

    virtual bool connectionHasNameOfEndpoints() const {
        return true;
    }

    virtual Contact detectNameServer(bool useDetectedServer,
                                     bool& scanNeeded,
                                     bool& serverUsed);

    virtual bool writeToNameServer(PortWriter& cmd,
                                   PortReader& reply,
                                   const ContactStyle& style);

private:
    void *system_resource;
    Contact contact;
};

class yarp::os::YarpDummyNameSpace : public YarpNameSpace {
public:
    YarpDummyNameSpace() : YarpNameSpace(Contact()) {}

    virtual bool localOnly() const {
        return true;
    }

    virtual Contact getNameServerContact() const {
        return Contact("/root");
    }
 };

#endif // YARP_OS_YARPNAMESPACE_H
