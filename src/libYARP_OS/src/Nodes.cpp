/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Nodes.h>
#include <yarp/os/Mutex.h>
#include <yarp/os/NestedContact.h>
#include <yarp/os/Node.h>
#include <yarp/os/Log.h>

#include <map>

using namespace yarp::os;

class yarp::os::Nodes::Private
{
public:
    Private();
    ~Private();

    void clear();

    Node* getNode(const ConstString& name, bool create);

    void add(Contactable& contactable);
    void update(Contactable& contactable);
    void prepare(const ConstString& name);
    void remove(Contactable& contactable);
    Contact query(const ConstString& name, const ConstString& category);
    void interrupt();

    bool enable(bool flag);

    Contact getParent(const ConstString& name);
    Contact getURI(const ConstString& name);

    void setActiveName(const ConstString& name);
    ConstString getActiveName();
    bool requireActiveName();

    void addExternalNode(const ConstString& name, Node& node);
    void removeExternalNode(const ConstString& name);

    // Port name
    // Pointer to Node
    // true = is external
    std::map<ConstString, std::pair<Node*, bool>> nodes_map;

    Mutex mutex;
    bool active;
    ConstString active_name;
    Node* dummy;
};

yarp::os::Nodes::Private::Private() :
        active(false),
        dummy(nullptr)
{
    clear();
}

yarp::os::Nodes::Private::~Private()
{
    clear();
}

void yarp::os::Nodes::Private::clear()
{
    mutex.lock();
    for (auto& n : nodes_map) {
        if (n.second.first) {
            if (!n.second.second) {
                delete n.second.first;
                n.second.first = nullptr;
            }
        }
    }
    nodes_map.clear();
    mutex.unlock();

    active = true;
    active_name = "";
    if (dummy != nullptr) {
        delete dummy;
        dummy = nullptr;
    }
}

Node* yarp::os::Nodes::Private::getNode(const ConstString& name, bool create)
{
    NestedContact nc(name);
    if (!nc.isNested()) {
        return nullptr;
    }
    Node* node = nullptr;
    mutex.lock();
    auto it = nodes_map.find(nc.getNodeName());
    mutex.unlock();
    if (it == nodes_map.end()) {
        if (create) {
            // The Node constructor ends up in locking again the the mutex,
            // hence it must be outside the lock
            node = new Node();
            mutex.lock();
            it = nodes_map.find(nc.getNodeName());
            if (it == nodes_map.end()) {
                nodes_map[nc.getNodeName()] = std::make_pair(node, false);
                node->prepare(nc.getNodeName());
            } else {
                // The node was not created by some other thread while this
                // thread was waiting on the lock.
                delete node;
                node = it->second.first;
            }
            mutex.unlock();
        }
    } else {
        node = it->second.first;
    }
    mutex.unlock();
    return node;
}

void yarp::os::Nodes::Private::add(Contactable& contactable)
{
    NestedContact nc(contactable.getName());
    if (!nc.isNested()) {
        return;
    }
    if (!active) {
        return;
    }
    Node* node = getNode(contactable.getName(), true);
    if (node) {
        node->add(contactable);
    }
}

void yarp::os::Nodes::Private::update(Contactable& contactable)
{
    NestedContact nc(contactable.getName());
    if (!nc.isNested()) {
        return;
    }
    if (!active) {
        return;
    }
    Node* node = getNode(contactable.getName(), true);
    if (node) {
        node->update(contactable);
    }
}

void yarp::os::Nodes::Private::prepare(const ConstString& name)
{
    NestedContact nc(name);
    if (!nc.isNested()) {
        return;
    }
    if (!active) {
        return;
    }
    getNode(name, true);
}

void yarp::os::Nodes::Private::remove(Contactable& contactable)
{
    if (!active) {
        return;
    }
    Node* node = getNode(contactable.getName(), false);
    if (node) {
        node->remove(contactable);
    }
}

Contact yarp::os::Nodes::Private::query(const ConstString& name, const ConstString& category)
{
    if (!active) {
        return Contact();
    }
    Contact result;
    mutex.lock();
    for (const auto& n : nodes_map) {
        result = n.second.first->query(name, category);
        if (result.isValid()) {
            break;
        }
    }
    mutex.unlock();
    return result;
}

void yarp::os::Nodes::Private::interrupt()
{
    if (!active) {
        return;
    }
    for (const auto& n : nodes_map) {
        n.second.first->interrupt();
    }
}

bool yarp::os::Nodes::Private::enable(bool flag)
{
    if (!flag) {
        clear();
    }
    active = flag;
    return active;
}

Contact yarp::os::Nodes::Private::getParent(const ConstString& name)
{
    Contact result;
    NestedContact nc(name);
    mutex.lock();
    auto it = nodes_map.find(nc.getNodeName());
    if (it != nodes_map.end()) {
        result = it->second.first->where();
    }
    mutex.unlock();
    return result;
}
Contact yarp::os::Nodes::Private::getURI(const ConstString& name)
{
    Contact result;
    NestedContact nc(name);
    mutex.lock();
    auto it = nodes_map.find(nc.getNodeName());
    if (it != nodes_map.end()) {
        result = it->second.first->query(nc.getNestedName());
    }
    mutex.unlock();
    return result;
}

void yarp::os::Nodes::Private::setActiveName(const ConstString& name)
{
    nodes_map[name].second = true;
    active_name = name;
}

ConstString yarp::os::Nodes::Private::getActiveName()
{
    return active_name;
}

bool yarp::os::Nodes::Private::requireActiveName()
{
    if (active_name == "") {
        dummy = new Node("...");
    }
    return true;
}

void yarp::os::Nodes::Private::addExternalNode(const ConstString& name, Node& node)
{
    mutex.lock();
    yAssert(nodes_map.find(name) == nodes_map.end());
    nodes_map[name] = std::make_pair(&node, true);
    mutex.unlock();
}

void yarp::os::Nodes::Private::removeExternalNode(const ConstString& name)
{
    mutex.lock();
    nodes_map.erase(name);
    mutex.unlock();
}


Nodes::Nodes() :
        mPriv(new yarp::os::Nodes::Private())
{
}

Nodes::~Nodes()
{
    delete mPriv;
}

void Nodes::add(Contactable& contactable)
{
    mPriv->add(contactable);
}

void Nodes::remove(Contactable& contactable)
{
    mPriv->remove(contactable);
}

Contact Nodes::query(const ConstString& name, const ConstString& category)
{
    return mPriv->query(name, category);
}

void Nodes::interrupt()
{
    mPriv->interrupt();
}

bool Nodes::enable(bool flag)
{
    return mPriv->enable(flag);
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
    mPriv->prepare(name);
}

void Nodes::update(Contactable& contactable)
{
    mPriv->update(contactable);
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
    mPriv->addExternalNode(name, node);
}

void Nodes::removeExternalNode(const ConstString& name)
{
    mPriv->removeExternalNode(name);
}
