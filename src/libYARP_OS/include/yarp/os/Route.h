// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_ROUTE_
#define _YARP2_ROUTE_

#include <yarp/os/api.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Contact.h>

namespace yarp {
    namespace os {
        class Route;
    }
}

/**
 * Information about a connection between two ports.
 * Contains the names of the endpoints, and the name of
 * the carrier in use between them.
 */
class YARP_OS_API yarp::os::Route {
public:
    /**
     *
     * Constructor.
     *
     */
    Route() {}

    /**
     * Create a route.
     *
     * @param fromKey Source of route.
     * @param toKey Destination of route.
     * @param carrier Type of carrier.
     */
    Route(const ConstString& fromKey,
          const ConstString& toKey,
          const ConstString& carrier) :
            fromKey(fromKey),
            toKey(toKey),
            carrier(carrier) {
    }

    /**
     * Copy constructor
     *
     * @param alt Route to copy.
     */
    Route(const Route& alt) :
            fromKey(alt.fromKey),
            toKey(alt.toKey),
            fromContact(alt.fromContact),
            toContact(alt.toContact),
            carrier(alt.carrier) {
    }

    /**
     * Get the source of the route.
     *
     * @return the source of the route (a port name)
     */
    const ConstString& getFromName() const {
        return fromKey;
    }

    /**
     * Get the destination of the route.
     *
     *
     * @return the destination of the route (a port name)
     */
    const ConstString& getToName() const {
        return toKey;
    }


    /**
     * Get the destination contact of the route, if avaiable
     *
     *
     * @return the destination of the route as a contact
     */
    const Contact& getToContact() const {
        return toContact;
    }

    /**
     * Get the carrier type of the route.
     *
     *
     * @return the carrier type of the route.
     */
    const ConstString& getCarrierName() const {
        return carrier;
    }

    /**
     * Copy this route with a different source.
     *
     * @param fromName The new source of the route.
     *
     * @return the created route.
     */
    Route addFromName(const ConstString& fromName) const {
        Route result(*this);
        result.fromKey = fromName;
        return result;
    }

    /**
     * Copy this route with a different destination.
     *
     * @param toName The new destination of the route.
     *
     * @return the created route.
     */
    Route addToName(const ConstString& toName) const {
        Route result(*this);
        result.toKey = toName;
        return result;
    }

    /**
     * Copy this route with a different contact.
     *
     * @param toContact new destination contact of the route.
     *
     * @return the created route.
     */
    Route addToContact(const Contact& toContact) const {
        Route result(*this);
        result.toContact = toContact;
        return result;
    }

    /**
     * Copy this route with a different carrier.
     *
     * @param carrierName The new carrier of the route.
     *
     * @return the created route.
     */
    Route addCarrierName(const ConstString& carrierName) const {
        Route result(*this);
        result.carrier = carrierName;
        return result;
    }

    /**
     * Render a text form of the route, "source->carrier->dest"
     *
     * @return the route in text form.
     */
    ConstString toString() const {
        return getFromName() + "->" + getCarrierName() + "->" +
            getToName();
    }

private:
    ConstString fromKey;
    ConstString toKey;
    Contact fromContact;
    Contact toContact;
    ConstString carrier;
};

#endif

