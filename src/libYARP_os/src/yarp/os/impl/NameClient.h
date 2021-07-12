/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_NAMECLIENT_H
#define YARP_OS_IMPL_NAMECLIENT_H

#include <yarp/os/Contact.h>
#include <yarp/os/ContactStyle.h>
#include <yarp/os/Nodes.h>

namespace yarp {
namespace os {

class Bottle;
class NameStore;

namespace impl {

class NameServer;

/**
 * Client for YARP name server.  There is one global client available
 * from the getNameClient method.  The protocol spoken by the name
 * client is rather old; there are simpler ways of talking to the
 * name server these days - it is now a regular port, that can read
 * and respond to messages in the bottle format.
 */
class YARP_os_impl_API NameClient
{
private:
    NameClient();
    NameClient(const NameClient& nic);

public:
    /**
     * Destructor
     */
    virtual ~NameClient();

    /**
     * Get an instance of the name client.
     *
     * This is a global singleton, created on demand.
     *
     * @return the name client
     */
    static NameClient& getNameClient();

    static NameClient* create();

    /**
     * The address of the name server.
     * @return the address of the name server
     */
    Contact getAddress();

    /**
     * Look up the address of a named port.
     * @param name the name of the port
     * @return the address associated with the port
     */
    Contact queryName(const std::string& name);

    /**
     * Register a port with a given name.
     * @param name the name of the port
     * @return the address associated with the port
     */
    Contact registerName(const std::string& name);

    /**
     * Register a port with a given name and a partial address.
     * @param name the name of the port
     * @param suggest a partially completed address
     * @return the address associated with the port
     */
    Contact registerName(const std::string& name, const Contact& suggest);

    /**
     * Register disassociation of name from port.
     * @param name the name to remove
     * @return the new result of queries for that name (should be empty)
     */
    Contact unregisterName(const std::string& name);

    /**
     * Send a message to the name server, and interpret the result as
     * an address.
     *
     * @param cmd the message to send (in text form)
     *
     * @return the address extracted from the reply, all errors result
     * in a non-valid address.
     */
    Contact probe(const std::string& cmd);

    /**
     * Extract an address from its text representation.
     *
     * @param txt the text representation of an address
     *
     * @return the address corresponding to the text representation
     */
    static Contact extractAddress(const std::string& txt);

    static Contact extractAddress(const Bottle& bot);

    /**
     * Send a text message to the nameserver, and return the result.
     *
     * @param cmd the message to send.
     * @param multi whether to expect a multi-line response.
     *
     * @return the reply from the name server.
     */
    std::string send(const std::string& cmd, bool multi = true, const ContactStyle& style = ContactStyle());

    /**
     * Send a message to the nameserver in Bottle format, and return the
     * result.
     *
     * @param[in] cmd the message to send.
     * @param[out] the reply from the name server.
     *
     * @return true on success.
     */
    bool send(yarp::os::Bottle& cmd, yarp::os::Bottle& reply);

    /**
     * For testing, the nameclient can be set to use a "fake" name server
     * rather than communicating with an external name server.
     *
     * @param fake whether to use a fake name server
     */
    void setFakeMode(bool fake = true);

    /**
     * Check whether a fake name server is being used.
     *
     * @return true iff a fake name server is being used.
     */
    bool isFakeMode() const;

    /**
     * Control whether the name client should scan for the name server
     * if the cached connection information for it is inaccurate
     *
     * @param allow true if the name client may scan for the name server.
     */
    void setScan(bool allow = true);

    /**
     * Control whether the name client can save the address of the name
     * server in a cache file
     *
     * @param allow true if the name client may save the name server address.
     */
    void setSave(bool allow = true);

    /**
     * Check whether the name client scanned for the address of the name
     * server.
     *
     * @return true iff the name client scanned for the name server.
     */
    bool didScan();

    /**
     * Check whether the name client saved the address of the name
     * server.
     *
     * @return true iff the name client saved the address of the name server.
     */
    bool didSave();

    /**
     * Force the name client to reread the cached location of the
     * name server.
     *
     * @return true if the address for the name server was found.
     */
    bool updateAddress();

    bool setContact(const yarp::os::Contact& contact);

    void queryBypass(NameStore* store);

    NameStore* getQueryBypass();

    std::string getMode();

    yarp::os::Nodes& getNodes();

private:
    Contact address;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) host;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) mode;
    bool fake;
    NameServer* fakeServer;
    bool allowScan;
    bool allowSaveScan;
    bool reportScan;
    bool reportSaveScan;
    bool isSetup;
    NameStore* altStore;
    yarp::os::Nodes nodes;

    NameServer& getServer();
    void setup();
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_NAMECLIENT_H
