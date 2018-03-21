/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_YARPNAMESPACE_H
#define YARP_OS_YARPNAMESPACE_H

#include <yarp/os/NameSpace.h>

#include <cstdio>

namespace yarp {
    namespace os {
        class YarpNameSpace;
        class YarpDummyNameSpace;
    }
}

class YARP_OS_API yarp::os::YarpNameSpace : public NameSpace
{
public:
    YarpNameSpace(const Contact& contact);

    virtual ~YarpNameSpace();

    virtual Contact getNameServerContact() const override
    {
        return contact;
    }

    virtual Contact queryName(const ConstString& name) override;

    virtual Contact registerName(const ConstString& name) override;

    virtual Contact registerContact(const Contact& contact) override;

    virtual Contact unregisterName(const ConstString& name) override;

    virtual Contact unregisterContact(const Contact& contact) override;

    virtual bool setProperty(const ConstString& name, const ConstString& key,
                             const Value& value) override;

    virtual Value *getProperty(const ConstString& name, const ConstString& key) override;

    virtual bool connectPortToTopic(const Contact& src,
                                    const Contact& dest,
                                    ContactStyle style)  override
    {
        return connectTopic("subscribe", false, true, src, dest, style);
    }

    virtual bool connectTopicToPort(const Contact& src,
                                    const Contact& dest,
                                    ContactStyle style) override
    {
        return connectTopic("subscribe", true, false, src, dest, style);
    }

    virtual bool disconnectPortFromTopic(const Contact& src,
                                         const Contact& dest,
                                         ContactStyle style) override
    {
        return connectTopic("unsubscribe", false, true, src, dest, style);
    }

    virtual bool disconnectTopicFromPort(const Contact& src,
                                         const Contact& dest,
                                         ContactStyle style) override
    {
        return connectTopic("unsubscribe", true, false, src, dest, style);
    }

    virtual bool connectPortToPortPersistently(const Contact& src,
                                               const Contact& dest,
                                               ContactStyle style) override
    {
        return connectTopic("subscribe", false, false, src, dest, style);
    }

    virtual bool disconnectPortToPortPersistently(const Contact& src,
                                                  const Contact& dest,
                                                  ContactStyle style) override
    {
        return connectTopic("unsubscribe", false, false, src, dest, style);
    }

    virtual bool connectTopic(const ConstString& dir,
                              bool srcIsTopic,
                              bool destIsTopic,
                              const Contact& src,
                              const Contact& dest,
                              ContactStyle style)
    {
        YARP_UNUSED(srcIsTopic);
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
                fprintf(stderr, "Failure: name server did not accept connection to topic.\n");
            }
        }
        return !fail;
    }

    virtual bool localOnly() const override
    {
        return false;
    }

    virtual bool usesCentralServer() const override
    {
        return true;
    }

    virtual bool serverAllocatesPortNumbers() const override
    {
        return true;
    }

    virtual bool connectionHasNameOfEndpoints() const override
    {
        return true;
    }

    virtual Contact detectNameServer(bool useDetectedServer,
                                     bool& scanNeeded,
                                     bool& serverUsed) override;

    virtual bool writeToNameServer(PortWriter& cmd,
                                   PortReader& reply,
                                   const ContactStyle& style) override;

private:
    void *system_resource;
    Contact contact;
};

class yarp::os::YarpDummyNameSpace : public YarpNameSpace
{
public:
    YarpDummyNameSpace() : YarpNameSpace(Contact())
    {
    }

    virtual bool localOnly() const override
    {
        return true;
    }

    virtual Contact getNameServerContact() const override
    {
        return Contact("/root");
    }
 };

#endif // YARP_OS_YARPNAMESPACE_H
