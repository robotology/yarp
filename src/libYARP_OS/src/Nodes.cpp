// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Nodes.h>
#include <yarp/os/Node.h>
#include <yarp/os/NestedContact.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Mutex.h>

#include <list>
#include <map>

using namespace yarp::os;
using namespace yarp::os::impl;

class NodesHelper {
public:
    Mutex mutex;
    std::map<ConstString,Node *> by_name;
    std::map<ConstString,int> is_external;
    bool active;
    ConstString active_name;

    NodesHelper() {
        active = true;
        clear();
        active_name = "";
    }

    ~NodesHelper() {
        clear();
    }

    void clear() {
        for (std::map<ConstString,Node *>::const_iterator it = by_name.begin();
             it != by_name.end(); it++) {
            if (it->second) delete it->second;
        }
        by_name.clear();
    }

    Node *getNode(const ConstString& name, bool create);

    void add(Contactable& contactable);
    void update(Contactable& contactable);
    void prepare(const ConstString& name);
    void remove(Contactable& contactable);
    Contact query(const ConstString& name,const ConstString& category);
    void interrupt();

    bool enable(bool flag) {
        if (!flag) clear();
        active = flag;
        return active;
    }

    Contact getParent(const ConstString& name) {
        Contact result;
        mutex.lock();
        NestedContact nc;
        nc.fromString(name);
        std::map<ConstString,Node *>::const_iterator it = by_name.find(nc.getNodeName());
        if (it!=by_name.end()) {
            result = it->second->where();
        }
        mutex.unlock();
        return result;
    }

    void setActiveName(const ConstString& name) {
        is_external[name] = 1;
        active_name = name;
    }

    ConstString getActiveName() {
        return active_name;
    }

    void addExternalNode(const ConstString& name, Node& node) {
        YARP_ASSERT(by_name.find(name)==by_name.end());
        is_external[name] = 1;
        by_name[name] = &node;
    }
    
    void removeExternalNode(const ConstString& name) {
        is_external.erase(name);
        by_name.erase(name);
    }
};

Node *NodesHelper::getNode(const ConstString& name, bool create) {
    NestedContact nc(name);
    if (!nc.isNested()) return NULL;
    std::map<ConstString,Node *>::const_iterator it = by_name.find(nc.getNodeName());
    Node *node = NULL;
    if (it == by_name.end()) {
        if (create) {
            node = new Node();
            YARP_ASSERT(node!=NULL);
            by_name[nc.getNodeName()] = node;
            node->prepare(nc.getNodeName());
        }
    } else {
        node = it->second;
    }
    return node;
}

void NodesHelper::add(Contactable& contactable) {
    if (!active) return;
    Node *node = getNode(contactable.getName(),true);
    if (node) node->add(contactable);
}

void NodesHelper::update(Contactable& contactable) {
    if (!active) return;
    Node *node = getNode(contactable.getName(),true);
    if (node) node->update(contactable);
}

void NodesHelper::prepare(const ConstString& name) {
    if (!active) return;
    getNode(name,true);
}

void NodesHelper::remove(Contactable& contactable) {
    if (!active) return;
    Node *node = getNode(contactable.getName(),false);
    if (node) node->remove(contactable);
}

Contact NodesHelper::query(const ConstString& name,const ConstString& category) {
    Contact result;
    if (!active) return result;
    for (std::map<ConstString,Node *>::const_iterator it = by_name.begin();
         it != by_name.end(); it++) {
        result = it->second->query(name,category);
        if (result.isValid()) return result;
    }
    return result;
}

void NodesHelper::interrupt() {
    if (!active) return;
    for (std::map<ConstString,Node *>::const_iterator it = by_name.begin();
         it != by_name.end(); it++) {
        it->second->interrupt();
    }
}


#define HELPER(x) (*((NodesHelper*)((x)->system_resource)))


Nodes::Nodes() {
    system_resource = new NodesHelper;
    YARP_ASSERT(system_resource!=NULL);
}


Nodes::~Nodes() {
    if (system_resource!=NULL) {
        delete &HELPER(this);
        system_resource = NULL;
    }
}


void Nodes::add(Contactable& contactable) {
    NestedContact nc(contactable.getName());
    if (!nc.isNested()) return;
    HELPER(this).mutex.unlock();
    HELPER(this).add(contactable);
    HELPER(this).mutex.lock();
}


void Nodes::remove(Contactable& contactable) {
    HELPER(this).mutex.lock();
    HELPER(this).remove(contactable);
    HELPER(this).mutex.unlock();
}


Contact Nodes::query(const ConstString& name,const ConstString& category) {
    HELPER(this).mutex.lock();
    Contact result = HELPER(this).query(name,category);
    HELPER(this).mutex.unlock();
    return result;
}

void Nodes::interrupt() {
    HELPER(this).interrupt();
}

bool Nodes::enable(bool flag) {
    HELPER(this).mutex.lock();
    bool result = HELPER(this).enable(flag);
    HELPER(this).mutex.unlock();
    return result;
}

void Nodes::clear() {
    HELPER(this).clear();
}

Contact Nodes::getParent(const ConstString& name) {
    return HELPER(this).getParent(name);
}

void Nodes::prepare(const ConstString& name) {
    NestedContact nc(name);
    if (!nc.isNested()) return;
    HELPER(this).mutex.unlock();
    HELPER(this).prepare(name);
    HELPER(this).mutex.lock();
}

void Nodes::update(Contactable& contactable) {
    NestedContact nc(contactable.getName());
    if (!nc.isNested()) return;
    HELPER(this).mutex.unlock();
    HELPER(this).update(contactable);
    HELPER(this).mutex.lock();
}


void Nodes::setActiveName(const ConstString& name) {
    HELPER(this).setActiveName(name);
}

ConstString Nodes::getActiveName() {
    return HELPER(this).getActiveName();
}


void Nodes::addExternalNode(const ConstString& name, Node& node) {
    HELPER(this).mutex.lock();
    HELPER(this).addExternalNode(name,node);
    HELPER(this).mutex.unlock();
}

void Nodes::removeExternalNode(const ConstString& name) {
    HELPER(this).mutex.lock();
    HELPER(this).removeExternalNode(name);
    HELPER(this).mutex.unlock();
}

