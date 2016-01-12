/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_NODE_H
#define YARP_OS_NODE_H

#include <yarp/os/Contactables.h>

namespace yarp {
    namespace os {
        class Node;
    }
}

class YARP_OS_API yarp::os::Node : public Contactables {
public:
    Node();
    Node(const ConstString& name);
    virtual ~Node();
    
    virtual void add(Contactable& contactable);
    virtual void update(Contactable& contactable);
    virtual void remove(Contactable& contactable);

    virtual Contact query(const ConstString& name,
                          const ConstString& category = "");

    virtual Contact where();

    void interrupt();

    virtual void prepare(const ConstString& name);
private:
    void *system_resource;
};

#endif // YARP_OS_NODE_H
