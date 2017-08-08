/*
 * Copyright (C) 2011 Istituto Italiano di Tecnologia (IIT)
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_MULTINAMESPACE_H
#define YARP_OS_MULTINAMESPACE_H

#include <yarp/os/NameSpace.h>
#include <yarp/os/NameStore.h>

namespace yarp {
    namespace os {
        class MultiNameSpace;
    }
}

class YARP_OS_API yarp::os::MultiNameSpace : public NameSpace {
public:
    MultiNameSpace();

    virtual ~MultiNameSpace();

    bool setLocalMode(bool flag);

    bool activate(bool force = false);

    virtual Contact getNameServerContact() const YARP_OVERRIDE;

    virtual Contact queryName(const ConstString& name) YARP_OVERRIDE;

    virtual Contact registerName(const ConstString& name) YARP_OVERRIDE;

    virtual Contact registerContact(const Contact& contact) YARP_OVERRIDE;

    virtual Contact unregisterName(const ConstString& name) YARP_OVERRIDE;

    virtual Contact unregisterContact(const Contact& contact) YARP_OVERRIDE;

    virtual bool setProperty(const ConstString& name, const ConstString& key,
                             const Value& value) YARP_OVERRIDE;

    virtual Value *getProperty(const ConstString& name, const ConstString& key) YARP_OVERRIDE;

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

    virtual bool localOnly() const YARP_OVERRIDE;

    virtual bool usesCentralServer() const YARP_OVERRIDE;

    virtual bool serverAllocatesPortNumbers() const YARP_OVERRIDE;

    virtual bool connectionHasNameOfEndpoints() const YARP_OVERRIDE;

    /**
     *
     * Set an alternative place to make name queries.
     * This method is typically used when writing name servers in
     * YARP, so you don't end up with a loop.
     *
     */
    virtual void queryBypass(NameStore *store);

    /**
     *
     * Get any alternative place to make name queries, if one
     * was set by queryBypass()
     *
     */
    virtual NameStore *getQueryBypass();

    virtual Contact detectNameServer(bool useDetectedServer,
                                     bool& scanNeeded,
                                     bool& serverUsed) YARP_OVERRIDE;

    virtual bool writeToNameServer(PortWriter& cmd,
                                   PortReader& reply,
                                   const ContactStyle& style) YARP_OVERRIDE;

private:
    void *system_resource;
    NameStore *altStore;
};

#endif // YARP_OS_MULTINAMESPACE_H
