/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

    virtual Contact getNameServerContact() const override;

    virtual Contact queryName(const std::string& name) override;

    virtual Contact registerName(const std::string& name) override;

    virtual Contact registerContact(const Contact& contact) override;

    virtual Contact unregisterName(const std::string& name) override;

    virtual Contact unregisterContact(const Contact& contact) override;

    virtual bool setProperty(const std::string& name, const std::string& key,
                             const Value& value) override;

    virtual Value *getProperty(const std::string& name, const std::string& key) override;

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

    virtual bool localOnly() const override;

    virtual bool usesCentralServer() const override;

    virtual bool serverAllocatesPortNumbers() const override;

    virtual bool connectionHasNameOfEndpoints() const override;

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
                                     bool& serverUsed) override;

    virtual bool writeToNameServer(PortWriter& cmd,
                                   PortReader& reply,
                                   const ContactStyle& style) override;

private:
    void *system_resource;
    NameStore *altStore;
};

#endif // YARP_OS_MULTINAMESPACE_H
