/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_ROUTE_H
#define YARP_OS_ROUTE_H

#include <yarp/conf/compiler.h>
#include <yarp/conf/system.h>
#include <yarp/os/api.h>

// Defined in this file:
namespace yarp { namespace os { class Route; }}

// Other forward declarations:
#ifndef YARP_WRAP_STL_STRING
# include <string>
namespace yarp { namespace os { typedef std::string ConstString; }}
#else
namespace yarp { namespace os { class ConstString; }}
#endif
namespace yarp { namespace os { class Contact; }}


namespace yarp {
namespace os {

/**
 * @ingroup comm_class
 * @brief Information about a connection between two ports.
 *
 * Contains the names of the endpoints, and the name of
 * the carrier in use between them.
 */
class YARP_OS_API Route {
public:

/** @{ */

    /**
     * @brief Default constructor.
     */
    Route();

    /**
     * @brief Create a route.
     *
     * @param fromName Source of route.
     * @param toName Destination of route.
     * @param carrier Type of carrier.
     */
    Route(const ConstString& fromName,
          const ConstString& toName,
          const ConstString& carrierName);

    /**
     * @brief Copy constructor
     *
     * @param rhs Route to copy.
     */
    Route(const Route& rhs);

    /**
     * @brief Move constructor.
     *
     * @param rhs the Route to be moved
     */
    Route(Route&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~Route();

    /**
     * Copy assignment operator.
     *
     * @param rhs the Route to copy
     * @return this object
     */
    Route& operator=(const Route& rhs);

    /**
     * @brief Move assignment operator.
     *
     * @param rhs the Route to be moved
     * @return this object
     */
    Route& operator=(Route&& rhs);

/** @} */
/** @{ */

    /**
     * @brief Get the source of the route.
     *
     * @return the source of the route (a port name)
     */
    const ConstString& getFromName() const;

    /**
     * @brief Set the source of the route.
     *
     * @param fromName the source of the route (a port name)
     */
    void setFromName(const ConstString& fromName);

/** @} */
/** @{ */

    /**
     * @brief Get the destination of the route.
     *
     * @return the destination of the route (a port name)
     */
    const ConstString& getToName() const;

    /**
     * @brief Set the destination of the route.
     *
     * @param toName the destination of the route (a port name)
     */
    void setToName(const ConstString& toName);

/** @} */
/** @{ */

    /**
     * @brief Get the destination contact of the route, if avaiable
     *
     * @return the destination of the route as a contact
     */
    const Contact& getToContact() const;

    /**
     * @brief Set the destination contact of the route
     *
     * @param toContact the destination of the route as a contact
     */
    void setToContact(const Contact& toContact);

/** @} */
/** @{ */

    /**
     * @brief Get the carrier type of the route.
     *
     * @return the carrier type of the route.
     */
    const ConstString& getCarrierName() const;

    /**
     * @brief Set the carrier type of the route.
     *
     * @param carrierName the carrier type of the route.
     */
    void setCarrierName(const ConstString& carrierName);

/** @} */
/** @{ */

    /**
     * @brief Swap from and to names
     */
    void swapNames();

/** @} */
/** @{ */

    /**
     * @brief Render a text form of the route, "source->carrier->dest"
     *
     * @return the route in text form.
     */
    ConstString toString() const;

/** @} */

#ifndef YARP_NO_DEPRECATED // Since YARP 2.3.70

/** @{ */

    /**
     * @brief Copy this route with a different source.
     *
     * @param fromName The new source of the route.
     * @return the created route.
     *
     * @deprecated since YARP 2.3.70
     */
    YARP_DEPRECATED_MSG("Use setFromName instead")
    Route addFromName(const ConstString& fromName) const;

    /**
     * @brief Copy this route with a different destination.
     *
     * @param toName The new destination of the route.
     * @return the created route.
     *
     * @deprecated since YARP 2.3.70
     */
    YARP_DEPRECATED_MSG("Use setToName instead")
    Route addToName(const ConstString& toName) const;

    /**
     * @brief Copy this route with a different contact.
     *
     * @param toContact new destination contact of the route.
     * @return the created route.
     *
     * @deprecated since YARP 2.3.70
     */
    YARP_DEPRECATED_MSG("Use setToConstact instead")
    Route addToContact(const Contact& toContact) const;

    /**
     * @brief Copy this route with a different carrier.
     *
     * @param carrierName The new carrier of the route.
     * @return the created route.
     *
     * @deprecated since YARP 2.3.70
     */
    YARP_DEPRECATED_MSG("Use setCarrierName instead")
    Route addCarrierName(const ConstString& carrierName) const;
#endif // YARP_NO_DEPRECATED

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private * mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_ROUTE_H
