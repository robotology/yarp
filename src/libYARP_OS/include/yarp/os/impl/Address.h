// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_ADDRESS_
#define _YARP2_ADDRESS_

#include <yarp/os/impl/String.h>
#include <yarp/os/Contact.h>

#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/PlatformStdio.h>

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
 */
class YARP_OS_impl_API yarp::os::impl::Address {
private:
    String name;
    int port;
    String carrier;
    String regName;
    float timeout;
public:


    /**
     * Simplest constructor.  The simplest type of Address is just a
     * machine name and a port, and an assumed protocol of tcp used
     * to communicate with that port.
     *
     * @param name Machine name - could be a hostname or ip address.
     * @param port Port number for socket communication.
     */
    Address(String name,
            int port) :
        name(name),
        port(port),
        carrier("tcp"),
        regName(""),
        timeout(-1) {
    }

    /**
     * Constructor with explicit protocol.
     *
     * @param name Machine name - could be a hostname or ip address.
     * @param port Port number for socket communication.
     * @param carrier The raw protocol used for communication.
     */
    Address(String name,
            int port,
            String carrier) :
        name(name),
        port(port),
        carrier(carrier),
        regName(""),
        timeout(-1) {
    }

    /**
     * Constructor with explicit protocol and registered name.
     *
     * @param name Machine name - could be a hostname or ip address.
     * @param port Port number for socket communication.
     * @param carrier The raw protocol used for communication.
     * @param regName A name associated with this Address in the global name server.
     */
    Address(String name,
            int port,
            String carrier,
            String regName) :
        name(name),
        port(port),
        carrier(carrier),
        regName(regName),
        timeout(-1) {
    }

    /**
     * Copy constructor.
     *
     * @param alt The Address to copy.
     */
    Address(const Address& alt) :
        name(alt.name),
        port(alt.port),
        carrier(alt.carrier),
        regName(alt.regName),
        timeout(-1) {
    }

    /**
     * Assignment operator
     *
     * @param alt The Address to copy.
     * @return A reference to this Address.
     */
    const Address& operator=(const Address& alt) {
        if (&alt != this) {
            name = alt.name;
            port = alt.port;
            carrier = alt.carrier;
            regName = alt.regName;
            timeout = alt.timeout;
        }

        return *this;
    }

    /**
     * Default constructor.  Creates an invalid Address.
     */
    Address() :
        name(""),
        port(-1),
        carrier(""),
        regName(""),
        timeout(-1) {
    }

    /**
     * Get machine name.
     *
     * @return Machine name - could be a hostname or ip address.
     */
    const String& getName() const {
        return name;
    }


    /**
     * Get port number.
     *
     * @return Port number for socket communication.
     */
    int getPort() const {
        return port;
    }

    /**
     * Get protocol.
     *
     * @return The raw protocol used for communication.
     */
    const String& getCarrierName() const {
        return carrier;
    }

    /**
     * Get registered name.
     *
     * @return The name associated with this Address in the global name server.
     */
    const String& getRegName() const {
        return regName;
    }

    /**
     * Render this Address textually, e.g. as something like udp://127.0.0.1:10001
     * (host 127.0.0.1, port 10001, protocol udp)
     *
     * @return Textual representation of this Address.
     */
    String toString() const {
        char buf[100];
#ifdef YARP_HAS_ACE
        ACE_OS::itoa(port, buf, 10);
#else
        snprintf(buf, sizeof(buf), "%d", port);
        buf[sizeof(buf)-1] = '\0';
#endif
        return carrier + "://" + name + ":" + buf;
    }

    /**
     * Check if this Address is valid - in other words, that it contains some
     * information.
     *
     * @return true iff this Address is valid.
     */
    bool isValid() const {
        return port>=0;
    }

    /**
     * Add a registered name to this Address.
     *
     * @param regName The registered name to add.
     * @return A new Address that is a copy of this, with the registered name set.
     */
    Address addRegName(const String& regName) const {
        return Address(name, port, carrier, regName);
    }

    /**
     * Add a machine name to this Address.
     *
     * @param name The machine name to add.
     * @return A new Address with the desired field added.
     */
    Address addName(const String& name) const {
        return Address(name, port, carrier, regName);
    }

    /**
     * Add a carrier name to this Address.
     *
     * @param carrier The carrier name to add.
     * @return A new Address with the desired field added.
     */
    Address addCarrier(const String& carrier) const {
        return Address(name, port, carrier, regName);
    }

    /**
     * Add socket information to this Address.
     *
     * @param carrier The carrier name to add.
     * @param name The machine name to add.
     * @param port The port number to add.
     * @return A new Address with the desired fields added.
     */
    Address addSocket(const String& carrier,
                      const String& name,
                      int port) const {
        return Address(name, port, carrier, regName);
    }

    /**
     * Check if this Address contains a registered port name.
     *
     * @return true iff this Address contains a registered port name.
     */
    bool hasRegName() const {
        return regName != "";
    }

    /**
     * Check if this Address contains a carrier name.
     *
     * @return true iff this Address contains a carrier name.
     */
    bool hasCarrierName() const {
        return carrier != "";
    }

    /**
     * Check if this Address has a timeout.
     *
     * @return true iff this Address has a timeout.
     */
    bool hasTimeout() const {
        return timeout >= 0;
    }

    /**
     * Set timeout for this Address.
     *
     * @param timeout The timeout to set.
     */
    void setTimeout(float timeout) {
        this->timeout = timeout;
    }

    /**
     * Get timeout for this Address.
     *
     * @return The timeout for this Address
     */
    float getTimeout() const {
        return timeout;
    }

    /**
     * Convert this Address to Contact, a simple wrapper
     *
     * @return the Address expressed as a contact
     */
    yarp::os::Contact toContact() const;

    /**
     * Convert Contact to an Address
     *
     * @param contact the Contact to convert
     * @return the Contact expressed as a Address
     */
    static Address fromContact(const yarp::os::Contact& contact);
};

#endif
