// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_ROUTE_
#define _YARP2_ROUTE_

#include <yarp/os/impl/String.h>

namespace yarp {
    namespace os {
        namespace impl {
            class Route;
        }
    }
}

/**
 * Information about a connection between two ports.
 * Contains the names of the endpoints, and the name of
 * the carrier in use between them.
 */
class YARP_OS_impl_API yarp::os::impl::Route {
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
    Route(const String& fromKey,
          const String& toKey,
          const String& carrier) {
        this->fromKey = fromKey;
        this->toKey = toKey;
        this->carrier = carrier;
    }

    /**
     * Copy constructor
     *
     * @param alt Route to copy.
     */
    Route(const Route& alt) {
        fromKey = alt.fromKey;
        toKey = alt.toKey;
        carrier = alt.carrier;
    }

    /**
     * Get the source of the route.
     *
     * @return the source of the route (a port name)
     */
    const String& getFromName() const {
        return fromKey;
    }

    /**
     * Get the destination of the route.
     *
     *
     * @return the destination of the route (a port name)
     */
    const String& getToName() const {
        return toKey;
    }

    /**
     * Get the carrier type of the route.
     *
     *
     * @return the carrier type of the route.
     */
    const String& getCarrierName() const {
        return carrier;
    }

    /**
     * Copy this route with a different source.
     *
     * @param fromName The new source of the route.
     *
     * @return the created route.
     */
    Route addFromName(const String& fromName) const {
        return Route(fromName,getToName(),getCarrierName());
    }

    /**
     * Copy this route with a different destination.
     *
     * @param fromName The new destination of the route.
     *
     * @return the created route.
     */
    Route addToName(const String& toName) const {
        return Route(getFromName(),toName,getCarrierName());
    }

    /**
     * Copy this route with a different carrier.
     *
     * @param fromName The new carrier of the route.
     *
     * @return the created route.
     */
    Route addCarrierName(const String& carrierName) const {
        return Route(getFromName(),getToName(),carrierName);
    }

    /**
     * Render a text form of the route, "source->carrier->dest"
     *
     * @return the route in text form.
     */
    String toString() const {
        return getFromName() + "->" + getCarrierName() + "->" +
            getToName();
    }

private:
    String fromKey;
    String toKey;
    String carrier;
};

#endif

