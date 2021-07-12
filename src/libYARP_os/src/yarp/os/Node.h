/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NODE_H
#define YARP_OS_NODE_H

#include <yarp/conf/compiler.h>

#include <yarp/os/Contactables.h>

namespace yarp {
namespace os {

/**
 * @brief The Node class
 *
 * A simple entity containing information about multiple contacts. Holds a port
 * representing the Node itself and a list of other contactables that represent
 * Publishers and Subscribers nested within the Node.
 */
class YARP_os_API Node : public Contactables
{
public:
    Node();
    Node(const std::string& name);
    virtual ~Node();

    /**
     * add a contactable to this node. If this node still has no defined name,
     * takes the name from the node specified in the contactable.
     * @param contactable to be adde
     */
    void add(Contactable& contactable) override;

    /**
     * remove specified contactable from the list of contactables associated
     * with this Node.
     * @param contactable to be removed
     */
    void remove(Contactable& contactable) override;

    /**
     * query the Node to obtain Contact information about a nested port
     * associated with this Node.
     * @param name the nestedName to query (see NestedContact.nestedName)
     * @param category of the contact to be queried (see NestedContact.category)
     * @return Contact with information about required port if found, empty
     *         Contact otherwise
     */
    virtual Contact query(const std::string& name,
                          const std::string& category = "") override;

    /**
     * update should update the contactable with new information.
     * @param contactable to be updated
     */
    void update(Contactable& contactable);

    /**
     * prepare if it is not already been done, opens the port of the Node.
     * @param name of the Node port to be opened
     */
    void prepare(const std::string& name);

    /**
     * interrupt delegates the call to the Node port interrupt.
     */
    void interrupt();

    /**
     * where getter fot information about the port of the Node.
     * @return a Contact with network information about this Node
     */
    Contact where();

private:
    class Helper;
    Helper* const mPriv;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_NODE_H
