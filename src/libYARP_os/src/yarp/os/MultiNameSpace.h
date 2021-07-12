/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_MULTINAMESPACE_H
#define YARP_OS_MULTINAMESPACE_H

#include <yarp/os/NameSpace.h>
#include <yarp/os/NameStore.h>

namespace yarp {
namespace os {

class YARP_os_API MultiNameSpace : public NameSpace
{
public:
    MultiNameSpace();

    virtual ~MultiNameSpace();

    bool setLocalMode(bool flag);

    bool activate(bool force = false);

    Contact getNameServerContact() const override;

    Contact queryName(const std::string& name) override;

    Contact registerName(const std::string& name) override;

    Contact registerContact(const Contact& contact) override;

    Contact unregisterName(const std::string& name) override;

    Contact unregisterContact(const Contact& contact) override;

    virtual bool setProperty(const std::string& name,
                             const std::string& key,
                             const Value& value) override;

    Value* getProperty(const std::string& name, const std::string& key) override;

    virtual bool connectPortToTopic(const Contact& src,
                                    const Contact& dest,
                                    const ContactStyle& style) override;

    virtual bool connectTopicToPort(const Contact& src,
                                    const Contact& dest,
                                    const ContactStyle& style) override;

    virtual bool disconnectPortFromTopic(const Contact& src,
                                         const Contact& dest,
                                         const ContactStyle& style) override;

    virtual bool disconnectTopicFromPort(const Contact& src,
                                         const Contact& dest,
                                         const ContactStyle& style) override;

    virtual bool connectPortToPortPersistently(const Contact& src,
                                               const Contact& dest,
                                               const ContactStyle& style) override;

    virtual bool disconnectPortToPortPersistently(const Contact& src,
                                                  const Contact& dest,
                                                  const ContactStyle& style) override;

    bool localOnly() const override;

    bool usesCentralServer() const override;

    bool serverAllocatesPortNumbers() const override;

    bool connectionHasNameOfEndpoints() const override;

    /**
     *
     * Set an alternative place to make name queries.
     * This method is typically used when writing name servers in
     * YARP, so you don't end up with a loop.
     *
     */
    virtual void queryBypass(NameStore* store);

    /**
     *
     * Get any alternative place to make name queries, if one
     * was set by queryBypass()
     *
     */
    virtual NameStore* getQueryBypass();

    virtual Contact detectNameServer(bool useDetectedServer,
                                     bool& scanNeeded,
                                     bool& serverUsed) override;

    virtual bool writeToNameServer(PortWriter& cmd,
                                   PortReader& reply,
                                   const ContactStyle& style) override;

private:
    void* system_resource;
    NameStore* altStore;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_MULTINAMESPACE_H
