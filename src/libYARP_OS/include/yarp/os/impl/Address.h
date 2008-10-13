// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_ADDRESS_
#define _YARP2_ADDRESS_

#include <yarp/String.h>
#include <yarp/os/Contact.h>

#include <ace/OS_NS_stdlib.h>

namespace yarp {
    namespace os {
        namespace impl {
            class Address;
        }
    }
}

/**
 * An abstraction for a location within the YARP network.
 * Should contain all the information need to connection
 * to that location from a peer.  This is currently
 * just a hostname, a port number, and the network protocol.
 * This may need to be extended for other systems, e.g. QNX.
 *
 */
class yarp::os::impl::Address {
private:
    yarp::String name, carrier, regName;
    int port;
public:


    /**
     * Simplest constructor.  The simplest type of address is just a 
     * machine name and a port, and an assumed protocol of tcp used
     * to communicate with that port.
     * @param name Machine name - could be a hostname or ip address.
     * @param port Port number for socket communication.
     */
    Address(yarp::String name,
            int port) {
        this->name = name;
        this->port = port;
        this->carrier = "tcp";
        this->regName = "";
    }

    /**
     * Constructor with explicit protocol.
     * @param name Machine name - could be a hostname or ip address.
     * @param port Port number for socket communication.
     * @param carrier The raw protocol used for communication.
     */
    Address(yarp::String name,
            int port,
            yarp::String carrier) {
        this->name = name;
        this->port = port;
        this->carrier = carrier;
        this->regName = "";
    }

    /**
     * Constructor with explicit protocol and registered name.
     * @param name Machine name - could be a hostname or ip address.
     * @param port Port number for socket communication.
     * @param carrier The raw protocol used for communication.
     * @param regName A name associated with this address in the global name server.
     */
    Address(yarp::String name,
            int port,
            yarp::String carrier,
            yarp::String regName) {
        this->name = name;
        this->port = port;
        this->carrier = carrier;
        this->regName = regName;
    }

    /**
     * Copy constructor.
     * @param alt The address to copy.
     */
    Address(const Address& alt) {
        name = alt.name;
        port = alt.port;
        carrier = alt.carrier;
        regName = alt.regName;
    }

    const Address& operator = (const Address& alt) {
        name = alt.name;
        port = alt.port;
        carrier = alt.carrier;
        regName = alt.regName;
        return *this;
    }

    /**
     * Default constructor.  Creates an invalid address.
     */
    Address() {
        port = -1;
    }

    /**
     * Get machine name.
     * @return Machine name - could be a hostname or ip address.
     */
    const yarp::String& getName() const {
        return name;
    }


    /**
     * Get port number.
     * @return Port number for socket communication.
     */
    int getPort() const {
        return port;
    }

    /**
     * Get protocol.
     * @return The raw protocol used for communication.
     */
    const yarp::String& getCarrierName() const {
        return carrier;
    }

    /**
     * Get registered name.
     * @return The name associated with this address in the global name server.
     */
    const yarp::String& getRegName() const {
        return regName;
    }

    /**
     * Render address textually, e.g. as something like udp://127.0.0.1:10001
     * (host 127.0.0.1, port 10001, protocol udp)
     * @return Textual representation of address.
     */
    yarp::String toString() const {
        char buf[100];
        ACE_OS::itoa(port,buf,10);
        return carrier + "://" + name + ":" + buf;
    }

    /**
     * Check if address is valid - in other words, that it contains some 
     * information.
     * @return True if address is valid.
     */
    bool isValid() const {
        return port>=0;
    }

    /**
     * Add a registered name to an address.
     * @param regName The registered name to add.
     * @return A new address that is a copy of this, with the registered name set.
     */
    Address addRegName(const String& regName) const {
        return Address(name,port,carrier,regName);
    }

    Address addName(const String& name) const {
        return Address(name,port,carrier,regName);
    }

    Address addCarrier(const String& carrier) const {
        return Address(name,port,carrier,regName);
    }

    Address addSocket(const String& carrier, const String& name,
                      int port) const {
        return Address(name,port,carrier,regName);
    }

    bool hasRegName() const {
        return regName != "";
    }

    bool hasCarrierName() const {
        return carrier != "";
    }

    yarp::os::Contact toContact() const;
    static Address fromContact(const yarp::os::Contact& contact);
};

#endif

