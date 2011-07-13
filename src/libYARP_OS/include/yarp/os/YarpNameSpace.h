// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_YARPNAMESPACE_
#define _YARP2_YARPNAMESPACE_

#include <yarp/os/NameSpace.h>

#include <stdio.h>

namespace yarp {
    namespace os {
        class YarpNameSpace;
        class YarpDummyNameSpace;
    }
}

class yarp::os::YarpNameSpace : public NameSpace {
public:
    YarpNameSpace(const Contact& contact);

    virtual ~YarpNameSpace();

    virtual Contact getNameServerContact() const {
        return contact;
    }
    
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
                                    ContactStyle style) {
        return connectTopic("subscribe",false,src,dest,style);
    }

    virtual bool connectTopicToPort(const Contact& src, 
                                    const Contact& dest,
                                    ContactStyle style) {
        return connectTopic("subscribe",true,src,dest,style);
    }

    virtual bool disconnectPortFromTopic(const Contact& src, 
                                         const Contact& dest,
                                         ContactStyle style) {
        return connectTopic("unsubscribe",false,src,dest,style);
    }

    virtual bool disconnectTopicFromPort(const Contact& src, 
                                         const Contact& dest,
                                         ContactStyle style) {
        return connectTopic("unsubscribe",true,src,dest,style);
    }

    virtual bool connectTopic(const ConstString& dir,
                              bool srcIsTopic,
                              const Contact& src, 
                              const Contact& dest,
                              ContactStyle style) {
        Contact dynamicSrc = src;
        Contact dynamicDest = dest;
        Bottle cmd, reply;
        cmd.add(dir.c_str());
        if (style.carrier!="") {
            if (srcIsTopic) {
                dynamicDest = dynamicDest.addCarrier(style.carrier);
            } else {
                dynamicSrc = dynamicSrc.addCarrier(style.carrier);
            }
        }
        cmd.add(dynamicSrc.toString().c_str());
        cmd.add(dynamicDest.toString().c_str());
        bool ok = NetworkBase::write(getNameServerContact(),
                                     cmd,
                                     reply);
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
};

#endif
