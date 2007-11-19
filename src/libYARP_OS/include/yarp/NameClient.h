// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_NAMECLIENT_
#define _YARP2_NAMECLIENT_

#include <yarp/Address.h>
#include <yarp/os/Bottle.h>

namespace yarp {
    class NameClient;
    class NameServer;
}

/**
 * Client for YARP name server.  There is one global client available
 * from the getNameClient method.
 */
class yarp::NameClient {
public:
  
    /**
     * Get an instance of the name client.
     * return the name client
     */
    static NameClient& getNameClient() {
        if (instance==NULL) {
            instance = new NameClient();
        }
        return *instance;
    }

    // for memory management testing
    static void removeNameClient() {
        if (instance!=NULL) {
            delete instance;
            instance = NULL;
        }
    }


    /**
     * The address of the name server.
     * @return the address of the name server
     */
    Address getAddress() {
        return address;
    }

    String getNamePart(const String& name) {
        return name;
    }

    /**
     * Look up the address of a named port.
     * @param name the name of the port
     * @return the address associated with the port
     */
    Address queryName(const String& name);

    /**
     * Register a port with a given name.
     * @param name the name of the port
     * @return the address associated with the port
     */
    Address registerName(const String& name);

    /**
     * Register a port with a given name and a partial address.
     * @param name the name of the port
     * @param address a partially completed address
     * @return the address associated with the port
     */
    Address registerName(const String& name, const Address& address);

    /**
     * Register disassociation of name from port.
     * @param name the name to remove
     * @return the new result of queries for that name (should be empty)
     */
    Address unregisterName(const String& name);

    Address probe(const String& cmd) {
        String result = send(cmd);
        return extractAddress(result);
    }

    static Address extractAddress(const String& txt);

    String send(const String& cmd, bool multi = true);

    bool send(yarp::os::Bottle& cmd,
              yarp::os::Bottle& reply);

    void setFakeMode(bool fake = true) {
        this->fake = fake;
    }

    bool isFakeMode() {
        return fake;
    }

    virtual ~NameClient();


private:
    NameClient();

    NameClient(const NameClient& nic) {
        // make sure no-one tries to do this accidentally
    }

    NameServer& getServer();


    Address address;
    String host;
    String process;
    bool fake;
    NameServer *fakeServer;

    static NameClient *instance;
};

#endif
