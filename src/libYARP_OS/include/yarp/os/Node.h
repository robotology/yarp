/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_NODE_H
#define YARP_OS_NODE_H

#include <yarp/os/Contactables.h>
#include <yarp/conf/compiler.h>

namespace yarp {
    namespace os {
        class Node;
    }
}


class YARP_OS_API yarp::os::Node : public Contactables
{
public:
    Node();
    Node(const ConstString& name);
    virtual ~Node();

    virtual void add(Contactable& contactable) override;
    virtual void update(Contactable& contactable);
    virtual void remove(Contactable& contactable) override;

    virtual Contact query(const ConstString& name,
                          const ConstString& category = "") override;

    virtual Contact where();

    void interrupt();

    virtual void prepare(const ConstString& name);
private:
    class Helper;
    Helper * const mPriv;
};

#endif // YARP_OS_NODE_H
