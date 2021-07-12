/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NAMESPACE_H
#define YARP_OS_NAMESPACE_H

#include <yarp/os/Contact.h>
#include <yarp/os/NameStore.h>
#include <yarp/os/Network.h>
#include <yarp/os/Value.h>

namespace yarp {
namespace os {

/**
 * An abstract name space for ports. This collects all the services
 * we may ask of a name server, to simplify plugging in alternate
 * implementations.
 */
class YARP_os_API NameSpace
{
public:
    /**
     * Destructor.
     */
    virtual ~NameSpace();

    /**
     * Check if a name space is available.
     * @return true if the name space is available, false otherwise.
     */
    virtual bool checkNetwork();

    /**
     * Check if a name space is available, with a timeout on any
     * network operations needed. If the timeout occurs, we
     * assume the name space is not available.
     *
     * @param[in] timeout time in seconds to wait for a response in any network operation needed.
     * @return true if the name server is available in the specified time, false otherwise.
     */
    virtual bool checkNetwork(double timeout);

    /**
     * Get an address for a name server that manages the name space,
     * if available.
     */
    virtual Contact getNameServerContact() const = 0;

    /**
     * Get the port name of a name server that manages the name space,
     * if available.
     */
    virtual std::string getNameServerName() const;

    /**
     * Map from port name to contact information.
     */
    virtual Contact queryName(const std::string& name) = 0;

    /**
     * Record contact information to tie to a port name.
     */
    virtual Contact registerName(const std::string& name) = 0;

    /**
     * Record contact information (should include a port name).
     */
    virtual Contact registerContact(const Contact& contact) = 0;

    /**
     * Disassociate contact information from a port name.
     */
    virtual Contact unregisterName(const std::string& name) = 0;

    /**
     * Disassociate contact information (should include a port name).
     */
    virtual Contact unregisterContact(const Contact& contact) = 0;

    /**
     * Record contact information, with access to the contact
     * information of other ports for cross-referencing.
     *
     * @param contact the contact information to record
     *
     * @param store an interface to port information as presented via
     * the YARP client API (as opposed to what a single NameSpace
     * would have access to).
     */
    virtual Contact registerAdvanced(const Contact& contact,
                                     NameStore* store)
    {
        YARP_UNUSED(store);
        return registerContact(contact);
    }

    /**
     * Remove contact information, with access to the contact
     * information of other ports for cross-referencing.
     */
    virtual Contact unregisterAdvanced(const std::string& name,
                                       NameStore* store)
    {
        YARP_UNUSED(store);
        return unregisterName(name);
    }

    /**
     * Associate a key/value pair with a named port.
     */
    virtual bool setProperty(const std::string& name,
                             const std::string& key,
                             const Value& value) = 0;

    /**
     * Get the value of a named key from a named port.
     *
     * @return nullptr if no value was set for the named key.
     */
    virtual Value *getProperty(const std::string& name,
                               const std::string& key) = 0;

    /**
     * Publish a port to a topic.
     */
    virtual bool connectPortToTopic(const Contact& src,
                                    const Contact& dest,
                                    const ContactStyle& style) = 0;

    /**
     * Subscribe a port to a topic.
     */
    virtual bool connectTopicToPort(const Contact& src,
                                    const Contact& dest,
                                    const ContactStyle& style) = 0;

    /**
     * Stop publishing a port to a topic.
     */
    virtual bool disconnectPortFromTopic(const Contact& src,
                                         const Contact& dest,
                                         const ContactStyle& style) = 0;

    /**
     * Stop subscribing a port to a topic.
     */
    virtual bool disconnectTopicFromPort(const Contact& src,
                                         const Contact& dest,
                                         const ContactStyle& style) = 0;

    /**
     * Connect two ports with persistence.
     */
    virtual bool connectPortToPortPersistently(const Contact& src,
                                               const Contact& dest,
                                               const ContactStyle& style) = 0;

    /**
     * Disconnect two ports, removing any persistence.
     */
    virtual bool disconnectPortToPortPersistently(const Contact& src,
                                                  const Contact& dest,
                                                  const ContactStyle& style) = 0;

    /**
     * Check if the NameSpace is only valid for the current process
     * ("local").
     */
    virtual bool localOnly() const = 0;

    /**
     * Check if a central server is involved in managing the NameSpace.
     */
    virtual bool usesCentralServer() const = 0;

    /**
     * Check if a central server is responsible for allocating port
     * numbers, or if this should be left up to the operating system.
     */
    virtual bool serverAllocatesPortNumbers() const = 0;

    /**
     * When connections are made involving ports managed by this NameSpace
     * do the ports involved end up knowing the names of their
     * counterparties?
     */
    virtual bool connectionHasNameOfEndpoints() const = 0;

    /**
     * Find a name server for this NameSpace, if applicable.
     *
     * @param useDetectedServer use any server found for future queries.
     * @param scanNeeded set to true if a search was needed, rather than
     * finding a name server based on cached hints.
     * @param serverUsed set to true if a server was found and marked for
     * use in future queries.
     */
    virtual Contact detectNameServer(bool useDetectedServer,
                                     bool& scanNeeded,
                                     bool& serverUsed) = 0;

    /**
     * Write a message to a name server for this NameSpace, if applicable.
     * Messages are name-server-specific.
     */
    virtual bool writeToNameServer(PortWriter& cmd,
                                   PortReader& reply,
                                   const ContactStyle& style) = 0;

};

} // namespace os
} // namespace yarp

#endif // YARP_OS_NAMESPACE_H
