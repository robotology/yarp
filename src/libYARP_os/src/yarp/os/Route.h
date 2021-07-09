/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_ROUTE_H
#define YARP_OS_ROUTE_H

#include <yarp/conf/compiler.h>

#include <yarp/os/api.h>

#include <string>

namespace yarp {
namespace os {

class Contact;

/**
 * @ingroup comm_class
 * @brief Information about a connection between two ports.
 *
 * Contains the names of the endpoints, and the name of
 * the carrier in use between them.
 */
class YARP_os_API Route
{
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
    Route(const std::string& fromName,
          const std::string& toName,
          const std::string& carrierName);

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
    Route(Route&& rhs) noexcept;

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
    Route& operator=(Route&& rhs) noexcept;

    /** @} */
    /** @{ */

    /**
     * @brief Get the source of the route.
     *
     * @return the source of the route (a port name)
     */
    const std::string& getFromName() const;

    /**
     * @brief Set the source of the route.
     *
     * @param fromName the source of the route (a port name)
     */
    void setFromName(const std::string& fromName);

    /** @} */
    /** @{ */

    /**
     * @brief Get the destination of the route.
     *
     * @return the destination of the route (a port name)
     */
    const std::string& getToName() const;

    /**
     * @brief Set the destination of the route.
     *
     * @param toName the destination of the route (a port name)
     */
    void setToName(const std::string& toName);

    /** @} */
    /** @{ */

    /**
     * @brief Get the destination contact of the route, if available
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
    const std::string& getCarrierName() const;

    /**
     * @brief Set the carrier type of the route.
     *
     * @param carrierName the carrier type of the route.
     */
    void setCarrierName(const std::string& carrierName);

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
    std::string toString() const;

    /** @} */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_ROUTE_H
