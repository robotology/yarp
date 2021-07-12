/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_ROSNAMESPACE_H
#define YARP_OS_ROSNAMESPACE_H

#include <yarp/os/Bottle.h>
#include <yarp/os/NameSpace.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>

#include <cstdio>
#include <mutex>

namespace yarp {
namespace os {

class YARP_os_API RosNameSpace : public NameSpace, public Thread
{
public:
    RosNameSpace(const Contact& contact);

    virtual ~RosNameSpace();

    Contact getNameServerContact() const override;

    Contact queryName(const std::string& name) override;
    Contact registerName(const std::string& name) override;
    Contact registerContact(const Contact& contact) override;
    Contact unregisterName(const std::string& name) override;
    Contact unregisterContact(const Contact& contact) override;
    virtual Contact registerAdvanced(const Contact& contact,
                                     NameStore* store) override;
    virtual Contact unregisterAdvanced(const std::string& name,
                                       NameStore* store) override;

    virtual bool setProperty(const std::string& name,
                             const std::string& key,
                             const Value& value) override;

    virtual Value* getProperty(const std::string& name,
                               const std::string& key) override;

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
    virtual bool connectTopic(Bottle& cmd,
                              bool srcIsTopic,
                              const Contact& src,
                              const Contact& dest,
                              const ContactStyle& style,
                              bool activeRegistration);

    bool localOnly() const override;
    bool usesCentralServer() const override;
    bool serverAllocatesPortNumbers() const override;
    bool connectionHasNameOfEndpoints() const override;

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
    static std::string toRosName(const std::string& name);
    static std::string fromRosName(const std::string& name);
    static std::string toRosNodeName(const std::string& name);
    static std::string fromRosNodeName(const std::string& name);
    static Contact rosify(const Contact& contact);

    void run() override;

private:
    Contact contact;
    Bottle pending;
    std::mutex mutex;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_ROSNAMESPACE_H
