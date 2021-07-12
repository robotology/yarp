/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NESTEDCONTACT_H
#define YARP_OS_NESTEDCONTACT_H

#include <yarp/os/api.h>

#include <string>

namespace yarp {
namespace os {

/**
 *
 * A placeholder for rich contact information.  Includes types, and
 * can express a ROS-style node/topic or node/service combination.
 *
 */
class YARP_os_API NestedContact
{
public:
    /** @{ */

    /**
     * @brief Default constructor.
     */
    NestedContact();

    /**
     * @brief Constructor.
     *
     * @param fullName the full name.
     */
    NestedContact(const std::string& fullName);

    /**
     * @brief Copy constructor.
     *
     * @param rhs the NestedContact to copy
     */
    NestedContact(const NestedContact& rhs);

    /**
     * @brief Move constructor.
     *
     * @param rhs the NestedContact to be moved
     */
    NestedContact(NestedContact&& rhs) noexcept;

    /**
     * @brief Destructor.
     */
    ~NestedContact();

    /**
     * Copy assignment operator.
     *
     * @param rhs the NestedContact to copy
     * @return this object
     */
    NestedContact& operator=(const NestedContact& rhs);

    /**
     * @brief Move assignment operator.
     *
     * @param rhs the NestedContact to be moved
     * @return this object
     */
    NestedContact& operator=(NestedContact&& rhs) noexcept;

    /** @} */
    /** @{ */

    bool fromString(const std::string& nFullName);

    void setTypeName(const std::string& nWireType);

    void setCategoryWrite();

    void setCategoryRead();

    std::string getFullName() const;

    std::string getNodeName() const;

    std::string getNestedName() const;

    std::string getCategory() const;

    std::string getTypeName() const;

    std::string getTypeNameStar() const;

    bool isNested() const;

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

#endif // YARP_OS_NESTEDCONTACT_H
