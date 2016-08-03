/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_NAMECLIENT
#define YARP2_NAMECLIENT

#include <yarp/os/Contact.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>
#include <yarp/os/NameStore.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Property.h>
#include <yarp/os/Mutex.h>
#include <yarp/os/Nodes.h>
#include <yarp/os/Network.h>

namespace yarp {
    namespace os {
        namespace impl {
            class NameClient;
            class NameServer;
        }
    }
}

/**
 * Client for YARP name server.  There is one global client available
 * from the getNameClient method.  The protocol spoken by the name
 * client is rather old; there are simpler ways of talking to the
 * name server these days - it is now a regular port, that can read
 * and respond to messages in the bottle format.
 */
class YARP_OS_impl_API yarp::os::impl::NameClient {
public:

    /**
     * Get an instance of the name client.  This is a global singleton,
     * created on demand.
     * return the name client
     */
    static NameClient& getNameClient() {
        mutex.lock();
        if (instance==NULL) {
            instance = new NameClient();
        }
        mutex.unlock();
        return *instance;
    }

    /**
     * Remove and delete the current global name client.
     *
     */
    static void removeNameClient() {
        mutex.lock();
        if (instance!=NULL) {
            delete instance;
            instance = NULL;
            instanceClosed = true;
        }
        mutex.unlock();
    }

    static bool isClosed() {
        return instanceClosed;
    }

    static NameClient *create() {
        return new NameClient();
    }

    /**
     * The address of the name server.
     * @return the address of the name server
     */
    Contact getAddress() {
        setup();
        return address;
    }

    /**
     * Deprecated, this is the identity function.
     *
     */
    ConstString getNamePart(const ConstString& name) {
        return name;
    }

    /**
     * Look up the address of a named port.
     * @param name the name of the port
     * @return the address associated with the port
     */
    Contact queryName(const ConstString& name);

    /**
     * Register a port with a given name.
     * @param name the name of the port
     * @return the address associated with the port
     */
    Contact registerName(const ConstString& name);

    /**
     * Register a port with a given name and a partial address.
     * @param name the name of the port
     * @param suggest a partially completed address
     * @return the address associated with the port
     */
    Contact registerName(const ConstString& name, const Contact& suggest);

    /**
     * Register disassociation of name from port.
     * @param name the name to remove
     * @return the new result of queries for that name (should be empty)
     */
    Contact unregisterName(const ConstString& name);

    /**
     * Send a message to the name server, and interpret the result as
     * an address.
     *
     * @param cmd the message to send (in text form)
     *
     * @return the address extracted from the reply, all errors result
     * in a non-valid address.
     */
    Contact probe(const ConstString& cmd) {
        ConstString result = send(cmd);
        return extractAddress(result);
    }

    /**
     * Extract an address from its text representation.
     *
     * @param txt the text representation of an address
     *
     * @return the address corresponding to the text representation
     */
    static Contact extractAddress(const ConstString& txt);

    static Contact extractAddress(const Bottle& bot);

    /**
     * Send a text message to the nameserver, and return the result.
     *
     * @param cmd the message to send.
     * @param multi whether to expect a multi-line response.
     *
     * @return the reply from the name server.
     */
    ConstString send(const ConstString& cmd, bool multi = true);

    /**
     * Send a message to the nameserver in Bottle format, and return the
     * result.
     *
     * @param cmd the message to send.
     *
     * @return the reply from the name server.
     */
    bool send(yarp::os::Bottle& cmd,
              yarp::os::Bottle& reply);

    /**
     * For testing, the nameclient can be set to use a "fake" name server
     * rather than communicating with an external name server.
     *
     * @param fake whether to use a fake name server
     */
    void setFakeMode(bool fake = true) {
        this->fake = fake;
    }

    /**
     * Check whether a fake name server is being used.
     *
     *
     * @return true iff a fake name server is being used.
     */
    bool isFakeMode() {
        return fake;
    }

    /**
     * Control whether the name client should scan for the name server
     * if the cached connection information for it is inaccurate
     *
     * @param allow true if the name client may scan for the name server.
     */
    void setScan(bool allow = true) {
        allowScan = allow;
    }

    /**
     * Control whether the name client can save the address of the name
     * server in a cache file
     *
     * @param allow true if the name client may save the name server address.
     */
    void setSave(bool allow = true) {
        allowSaveScan = allow;
    }

    /**
     * Check whether the name client scanned for the address of the name
     * server.
     *
     * @return true iff the name client scanned for the name server.
     */
    bool didScan() {
        return reportScan;
    }

    /**
     * Check whether the name client saved the address of the name
     * server.
     *
     * @return true iff the name client saved the address of the name server.
     */
    bool didSave() {
        return reportSaveScan;
    }

    /**
     * Force the name client to reread the cached location of the
     * name server.
     *
     * @return true if the address for the name server was found.
     */
    bool updateAddress();

    bool setContact(const yarp::os::Contact& contact);

    virtual ~NameClient();

    void queryBypass(NameStore *store) {
        altStore = store;
    }

    NameStore *getQueryBypass() {
        return altStore;
    }

    yarp::os::ConstString getMode() {
        return mode.c_str();
    }

    /**
     * Make a singleton resource finder available to YARP,
     * for finding configuration files.
     *
     */
    ResourceFinder& getResourceFinder() {
        return resourceFinder;
    }

    yarp::os::Nodes& getNodes() {
        return nodes;
    }

    Property& getPluginState() {
        return pluginState;
    }

private:
    NameClient();

    NameClient(const NameClient& nic) {
        // make sure no-one tries to do this accidentally
    }

    NameServer& getServer();


    Contact address;
    ConstString host;
    ConstString mode;
    bool fake;
    NameServer *fakeServer;
    bool allowScan;
    bool allowSaveScan;
    bool reportScan;
    bool reportSaveScan;
    bool isSetup;
    NameStore *altStore;
    yarp::os::ResourceFinder resourceFinder;
    yarp::os::Property pluginState;
    yarp::os::Nodes nodes;

    static yarp::os::Mutex mutex;
    static NameClient *instance;
    static bool instanceClosed;

    void setup();
};

#endif
