/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_NODES_H
#define YARP_OS_NODES_H

#include <yarp/os/Contactables.h>

namespace yarp {
namespace os {

class Node;

class YARP_OS_API Nodes : public yarp::os::Contactables
{
public:
    Nodes();
    virtual ~Nodes();

    virtual void add(Contactable& contactable) override;
    virtual void remove(Contactable& contactable) override;

    virtual Contact query(const std::string& name,
                          const std::string& category = "") override;

    virtual Contact getParent(const std::string& name);

    virtual Contact getURI(const std::string& name);

    virtual void prepare(const std::string& name);

    virtual void update(Contactable& contactable);

    void interrupt();

    bool enable(bool flag);
    void clear();
    void setActiveName(const std::string& name);
    void addExternalNode(const std::string& name, Node& node);
    void removeExternalNode(const std::string& name);
    std::string getActiveName();
    bool requireActiveName();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace yarp
} // namespace os


#endif // YARP_OS_NODES_H
