/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NODES_H
#define YARP_OS_NODES_H

#include <yarp/os/Contactables.h>

// forward declarations:
namespace yarp {
namespace os {
class Node;
} // namespace os
} // namespace yarp

namespace yarp {
namespace os {

/**
 * @brief The Nodes class
 *
 * A container for multiple nodes, keeping information about the currently
 * active node. When a Publisher or a Subscriber is registered, a NestedContact
 * will be created associating the topic to the active Node if present. The
 * active Node is the last instantiated in temporal order. Instantiation of
 * multiple Publisher/Subscriber associated with multiple Node is not thread
 * safe.
 */
class YARP_os_API Nodes : public yarp::os::Contactables
{
public:
    Nodes();
    virtual ~Nodes();

    /**
     * add a Contactable to the Node specified in the contactable name (see
     * NestedContact.nodeName). If the specified node is in this container, add
     * the contactable to it, otherwise firstly creates the node and add it to
     * this container.
     * @param contactable to be added
     */
    void add(Contactable& contactable) override;

    /**
     * remove a Contactable from the Node specified in the contactable's name.
     * @param contactable to be removed
     */
    void remove(Contactable& contactable) override;

    /**
     * query the list of Node to find a Contact with the specified name.
     * @param name of the Contact (see NestedContact.nestedName)
     * @param category of the Contact (see NestedContact.category)
     * @return first matching Contact or an empty one
     */
    virtual Contact query(const std::string& name,
                          const std::string& category = "") override;

    /**
     * update a Node information in this container.
     * @param contactable to be updated
     */
    virtual void update(Contactable& contactable);

    /**
     * getParent get info about node associated with the specified port.
     * @param name of a registered port (in the format /topic@/node)
     * @return Contact information about the /node port if found, empty Contact
     *         otherwise
     */
    Contact getParent(const std::string& name);

    /**
     * getURI queries the Node specified in the name parameter to get Contact
     * information about the specified port
     * @param name of a registered port (in the format /topic@/node)
     * @return Contact information about /topic@/node port or empty Contact
     */
    Contact getURI(const std::string& name);

    /**
     * prepare checks for the existence of the node specified in the name
     * parameter. If the node doesn't exists, creates it. If the name parameter
     * is not representing a NestedContact, does nothing.
     * @param name of a port in the format /topic@/node
     */
    void prepare(const std::string& name);

    /**
     * interrupt delegates interrupt call to all of the Node in this container.
     */
    void interrupt();

    /**
     * enable setter for the activity state of the container. If disabling
     * (parameter is false) clears the container.
     * @param flag
     * @return current activity state, after setting
     */
    bool enable(bool flag);

    /**
     * clear empties the container
     */
    void clear();

    /**
     * setActiveName setter for the currently active node
     * @param name of the Node to be set as active
     */
    void setActiveName(const std::string& name);

    /**
     * addExternalNode adds a Node to this container.
     * ! Do NOT add two times the same Node.
     * @param name of the node to be added
     * @param node to be added
     */
    void addExternalNode(const std::string& name, Node& node);

    /**
     * removeExternalNode erase the node from the container.
     * @param name of the Node to be removed
     */
    void removeExternalNode(const std::string& name);

    /**
     * getActiveName getter for the currently active node's name
     * @return the currently active node's name.
     */
    std::string getActiveName();

    /**
     * requireActiveName if there is no active node, creates a temporary one.
     * @return true
     */
    bool requireActiveName();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp


#endif // YARP_OS_NODES_H
