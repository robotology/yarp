/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_CONTACTABLES_H
#define YARP_OS_CONTACTABLES_H

#include <yarp/os/Contactable.h>

namespace yarp {
namespace os {

/**
 * @brief The yarp::os::Contactables class
 *
 * Generic interface for a container of `yarp::os::Contactable` objects.
 * Implemented by ROS related classes `yarp::os::Node` and `yarp::os::Nodes`.
 */
class YARP_os_API Contactables
{
public:
    virtual ~Contactables();

    /**
     * add a Contactable to the container.
     * @param contactable to be added
     */
    virtual void add(Contactable& contactable) = 0;

    /**
     * remove a Contactable from the container.
     * @param contactable to be removed
     */
    virtual void remove(Contactable& contactable) = 0;

    /**
     * query the container to obtain a specified contact.
     * @param name the name of the Contact
     * @param category the category of the Contact (tells if the Contact has a
     *        nested writer or reader, see NestedContact.category)
     * @return the first matched Contact with the specified name [and category]
     *         if found, an empty contact otherwise
     */
    virtual Contact query(const std::string& name,
                          const std::string& category = "") = 0;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_CONTACTABLES_H
