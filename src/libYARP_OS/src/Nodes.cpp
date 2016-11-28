/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/conf/compiler.h>
#include <yarp/os/Nodes.h>
#include <yarp/os/Node.h>
#include <yarp/os/NestedContact.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Mutex.h>

#include <list>
#include <map>

using namespace yarp::os;
using namespace yarp::os::impl;

class yarp::os::Nodes::Helper
{
public:
    Mutex mutex;
    std::map<ConstString,Node *> by_name;
    std::map<ConstString,int> is_external;
    bool active;
    ConstString active_name;
    Node *dummy;

    Helper() : dummy(YARP_NULLPTR)
    {
        clear();
    }

    ~Helper()
    {
        clear();
    }

    void clear()
    {
        std::map<ConstString,Node *> by_name_cp = by_name;
        std::map<ConstString,int> is_external_cp = is_external;
        for (std::map<ConstString,Node *>::const_iterator it = by_name_cp.begin();
             it != by_name_cp.end(); it++) {
            if (it->second) {
                bool ext = false;
                if (is_external_cp.find(it->first) != is_external_cp.end()) {
                    ext = is_external_cp[it->first];
                }
                if (!ext) delete it->second;
            }
        }
        by_name.clear();
        is_external.clear();
        active = true;
        active_name = "";
        if (dummy != YARP_NULLPTR) {
            delete dummy;
            dummy = YARP_NULLPTR;
        }
    }

    Node *getNode(const ConstString& name, bool create);

    void add(Contactable& contactable);
    void update(Contactable& contactable);
    void prepare(const ConstString& name);
    void remove(Contactable& contactable);
    Contact query(const ConstString& name,const ConstString& category);
    void interrupt();

    bool enable(bool flag)
    {
        if (!flag) {
            clear();
        }
        active = flag;
        return active;
    }

    Contact getParent(const ConstString& name)
    {
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

    Contact getURI(const ConstString& name)
    {
        Contact result;
        mutex.lock();
        NestedContact nc;
        nc.fromString(name);
        std::map<ConstString,Node *>::const_iterator it = by_name.find(nc.getNodeName());
        if (it!=by_name.end()) {
            result = it->second->query(nc.getNestedName());
        }
        mutex.unlock();
        return result;
    }

    void setActiveName(const ConstString& name)
    {
        is_external[name] = 1;
        active_name = name;
    }

    ConstString getActiveName()
    {
        return active_name;
    }

    bool requireActiveName()
    {
        if (active_name=="") {
            dummy = new Node("...");
        }
        return true;
    }

    void addExternalNode(const ConstString& name, Node& node)
    {
        yAssert(by_name.find(name)==by_name.end());
        is_external[name] = 1;
        by_name[name] = &node;
    }

    void removeExternalNode(const ConstString& name)
    {
        is_external.erase(name);
        by_name.erase(name);
    }
};

Node *yarp::os::Nodes::Helper::getNode(const ConstString& name, bool create)
{
    NestedContact nc(name);
    if (!nc.isNested()) {
        return YARP_NULLPTR;
    }
    std::map<ConstString,Node *>::const_iterator it = by_name.find(nc.getNodeName());
    Node *node = YARP_NULLPTR;
    if (it == by_name.end()) {
        if (create) {
            node = new Node();
            yAssert(node != YARP_NULLPTR);
            by_name[nc.getNodeName()] = node;
            node->prepare(nc.getNodeName());
        }
    } else {
        node = it->second;
    }
    return node;
}

void yarp::os::Nodes::Helper::add(Contactable& contactable)
{
    if (!active) {
        return;
    }
    Node *node = getNode(contactable.getName(),true);
    if (node) node->add(contactable);
}

void yarp::os::Nodes::Helper::update(Contactable& contactable)
{
    if (!active) {
        return;
    }
    Node *node = getNode(contactable.getName(),true);
    if (node) node->update(contactable);
}

void yarp::os::Nodes::Helper::prepare(const ConstString& name)
{
    if (!active) {
        return;
    }
    getNode(name,true);
}

void yarp::os::Nodes::Helper::remove(Contactable& contactable)
{
    if (!active) {
        return;
    }
    Node *node = getNode(contactable.getName(),false);
    if (node) node->remove(contactable);
}

Contact yarp::os::Nodes::Helper::query(const ConstString& name,const ConstString& category)
{
    Contact result;
    if (!active) {
        return result;
    }
    for (std::map<ConstString,Node *>::const_iterator it = by_name.begin();
         it != by_name.end(); it++) {
        result = it->second->query(name,category);
        if (result.isValid()) {
            return result;
        }
    }
    return result;
}

void yarp::os::Nodes::Helper::interrupt()
{
    if (!active) {
        return;
    }
    for (std::map<ConstString,Node *>::const_iterator it = by_name.begin();
         it != by_name.end(); it++) {
        it->second->interrupt();
    }
}




Nodes::Nodes() :
        mPriv(new yarp::os::Nodes::Helper)
{
    yAssert(mPriv != YARP_NULLPTR);
}


Nodes::~Nodes() {
        delete mPriv;
}


void Nodes::add(Contactable& contactable)
{
    NestedContact nc(contactable.getName());
    if (!nc.isNested()) {
        return;
    }
    mPriv->mutex.unlock();
    mPriv->add(contactable);
    mPriv->mutex.lock();
}


void Nodes::remove(Contactable& contactable)
{
    mPriv->mutex.lock();
    mPriv->remove(contactable);
    mPriv->mutex.unlock();
}


Contact Nodes::query(const ConstString& name,const ConstString& category)
{
    mPriv->mutex.lock();
    Contact result = mPriv->query(name,category);
    mPriv->mutex.unlock();
    return result;
}

void Nodes::interrupt()
{
    mPriv->interrupt();
}

bool Nodes::enable(bool flag)
{
    mPriv->mutex.lock();
    bool result = mPriv->enable(flag);
    mPriv->mutex.unlock();
    return result;
}

void Nodes::clear()
{
    mPriv->clear();
}

Contact Nodes::getParent(const ConstString& name)
{
    return mPriv->getParent(name);
}

Contact Nodes::getURI(const ConstString& name)
{
    return mPriv->getURI(name);
}

void Nodes::prepare(const ConstString& name)
{
    NestedContact nc(name);
    if (!nc.isNested()) {
        return;
    }
    mPriv->mutex.unlock();
    mPriv->prepare(name);
    mPriv->mutex.lock();
}

void Nodes::update(Contactable& contactable)
{
    NestedContact nc(contactable.getName());
    if (!nc.isNested()) {
        return;
    }
    mPriv->mutex.unlock();
    mPriv->update(contactable);
    mPriv->mutex.lock();
}


void Nodes::setActiveName(const ConstString& name)
{
    mPriv->setActiveName(name);
}

ConstString Nodes::getActiveName()
{
    return mPriv->getActiveName();
}

bool Nodes::requireActiveName()
{
    return mPriv->requireActiveName();
}

void Nodes::addExternalNode(const ConstString& name, Node& node)
{
    mPriv->mutex.lock();
    mPriv->addExternalNode(name,node);
    mPriv->mutex.unlock();
}

void Nodes::removeExternalNode(const ConstString& name)
{
    mPriv->mutex.lock();
    mPriv->removeExternalNode(name);
    mPriv->mutex.unlock();
}
