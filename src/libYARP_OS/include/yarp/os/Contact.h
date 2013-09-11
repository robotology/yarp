// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_CONTACT_
#define _YARP2_CONTACT_

#include <yarp/os/ConstString.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/NestedContact.h>

namespace yarp {
    namespace os {
        class Contact;
    }
}

/**
 * Represents how to reach a part of a YARP network.
 * May contain complete or partial information about network parameters.
 * Use the factory  methods (Contact::byName, Contact::bySocket etc.)
 * to create Contact objects.
 */
class YARP_OS_API yarp::os::Contact {
public:

    Contact(const ConstString& hostName, int port);

    /**
     * Constructor.  Returns a new, blank, unnamed contact.
     * @return a blank contact
     */
    static Contact empty();

    /**
     * Constructor.  Returns a new, invalid contact.
     * @return an invalid contact
     */
    static Contact invalid();

    /**
     * Constructor.  Returns a named contact, with no information about
     * how to reach that contact.
     * @param name the name for the contact
     * @return a named contact
     */
    static Contact byName(const ConstString& name);

    /**
     * Constructor.  Returns an unnamed contact, with information about
     * which carrier/protocol to use to reach that contact.
     * @param carrier the carrier to add ("tcp", "udp", ...)
     * @return the new contact
     */
    static Contact byCarrier(const ConstString& carrier);


    /**
     * Constructor.  Returns a contact configured from the information
     * in config.
     * @param config a Property, Bottle, or other Searchable containing the
     * contact configuration.  Key names include "name", "ip", "port_number",
     * "carrier"
     * @return the new contact
     */
    static Contact byConfig(Searchable& config);


    /**
     * Add the specified carrier to this Contact, and return the result.
     * @param carrier the carrier to add
     * @return the new contact
     */
    Contact addCarrier(const ConstString& carrier) const;

    Contact addHost(const ConstString& host) const;

    /**
     * Constructor for a socket contact.
     * Creates an unnamed contact, with information about
     * how to reach it using socket communication.
     * @param carrier the carrier (network protocol) to use
     * @param host the name of the host machine (usually expressed as an
     *             IP address)
     * @param portNumber the number of the socket port to use
     * @return an unnamed contact with socket communication parameters
     */
    static Contact bySocket(const ConstString& carrier,
                            const ConstString& host,
                            int portNumber);


    /**
     * Add information to a contact about
     * how to reach it using socket communication.
     * @param carrier the carrier (network protocol) to use
     * @param host the name of the host machine (usually expressed as an
     *             IP address)
     * @param portNumber the number of the socket port to use
     * @return the new contact with socket communication parameters
     */
    Contact addSocket(const ConstString& carrier,
                      const ConstString& host,
                      int portNumber) const;

    Contact addNested(const NestedContact& nc) const;

    /**
     *
     * Add the name of a contact.
     *
     */
    Contact addName(const ConstString& name) const;


    /**
     *
     * If the host is a machine name, convert it to a plausible IP address.
     *
     */
    static ConstString convertHostToIp(const char *name);

    /**
     * Copy constructor.
     * @param alt the contact to copy
     */
    Contact(const Contact& alt);

    /**
     * Assignment.
     * @param alt the contact to copy
     * @return this object
     */
    const Contact& operator = (const Contact& alt);

    /**
     * Destructor.
     */
    virtual ~Contact();

    /**
     * Get the name associated with this contact.
     * @return The name associated with this contact, or the empty string
     *         if no name is set
     */
    ConstString getName() const;

    /**
     * Get the host name associated with this contact for socket communication.
     * @return The host name associated with this contact, or the empty string
     *         if no host name is set
     */
    ConstString getHost() const;

    /**
     * Get the carrier associated with this contact for socket communication.
     * @return The carrier associated with this contact, or the empty string
     *         if no carrier is set
     */
    ConstString getCarrier() const;


    const NestedContact& getNested() const;


    /**
     * Get the port number associated with this contact for socket communication.
     * @return The port number associated with this contact, or <= 0
     *         if no port number is set
     */
    int getPort() const;

    /**
     * Get a textual representation of the contact
     * @return a textual representation of the contact.
     */
    ConstString toString() const;

    /**
     * Get a representation of the contact as a URI
     * @return a URI representation of the contact.
     */
    ConstString toURI() const;

    /**
     * Parse a textual representation of a contact.
     * @param txt the text to parse
     * @return the contact
     */
    static Contact fromString(const ConstString& txt);

    /**
     * Checks if a contact is tagged as valid.
     * @return true iff contact is tagged as valid.  All contact objects
     * are valid except the one created by Contact::invalid.
     */
    bool isValid() const;

    /**
     * Constructor.  Makes an empty contact.
     */
    Contact();


    /**
     * Check if this Contact has a timeout.
     *
     * @return true iff this Contact has a timeout.
     */
    bool hasTimeout() const;

    /**
     * Set timeout for this Contact.
     *
     * @param timeout The timeout to set.
     */
    void setTimeout(float timeout);

    void setNested(const yarp::os::NestedContact& flavor);

    /**
     * Get timeout for this Address.
     *
     * @return The timeout for this Address
     */
    float getTimeout() const;

    ConstString getRegName() const;

private:
    ConstString regName;
    ConstString hostName;
    ConstString carrier;
    NestedContact flavor;

    int port;
    float timeout;
};

#endif
