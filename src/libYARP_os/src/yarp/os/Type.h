/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_TYPE_H
#define YARP_OS_TYPE_H

#include <yarp/os/api.h>

#include <string>

namespace yarp {
namespace os {

class Property;
class Searchable;
class Value;

class YARP_os_API Type
{
public:
    /** @{ */

    /**
     * @brief Constructor.
     */
    Type();

    /**
     * @brief Copy constructor.
     *
     * @param rhs the Type to copy
     */
    Type(const Type& rhs);

    /**
     * @brief Move constructor.
     *
     * @param rhs the Type to be moved
     */
    Type(Type&& rhs) noexcept;

    /**
     * @brief Destructor.
     */
    virtual ~Type();

    /**
     * Copy assignment operator.
     *
     * @param rhs the Type to copy
     * @return this object
     */
    Type& operator=(const Type& rhs);

    /**
     * @brief Move assignment operator.
     *
     * @param rhs the Type to be moved
     * @return this object
     */
    Type& operator=(Type&& rhs) noexcept;

    /** @} */
    /** @{ */

    std::string getName() const;

    std::string getNameOnWire() const;

    bool hasName() const;

    bool isValid() const;

    std::string toString() const;

    const Searchable& readProperties() const;

    Property& writeProperties();

    Type& addProperty(const char* key, const Value& val);

    /** @} */
    /** @{ */

    static Type byName(const char* name);

    static Type byName(const char* name, const char* name_on_wire);

    static Type byNameOnWire(const char* name_on_wire);

    static Type anon();

    /** @} */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_TYPE_H
