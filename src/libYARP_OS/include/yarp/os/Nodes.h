/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_NODES_H
#define YARP_OS_NODES_H

#include <yarp/os/Contactables.h>

namespace yarp {
    namespace os {
        class Node;
        class Nodes;
    }
}

class YARP_OS_API yarp::os::Nodes : public Contactables
{
public:
    Nodes();
    virtual ~Nodes();

    virtual void add(Contactable& contactable);
    virtual void remove(Contactable& contactable);

    virtual Contact query(const ConstString& name,
                          const ConstString& category = "");

    virtual Contact getParent(const ConstString& name);

    virtual Contact getURI(const ConstString& name);

    virtual void prepare(const ConstString& name);

    virtual void update(Contactable& contactable);

    void interrupt();

    bool enable(bool flag);
    void clear();
    void setActiveName(const ConstString& name);
    void addExternalNode(const ConstString& name, Node& node);
    void removeExternalNode(const ConstString& name);
    ConstString getActiveName();
    bool requireActiveName();
private:
    class Helper;
    Helper * const mPriv;
};

#endif // YARP_OS_NODES_H
