/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Nodes.h>

#include <yarp/os/Log.h>
#include <yarp/os/NestedContact.h>
#include <yarp/os/Node.h>

#include <map>
#include <mutex>

using namespace yarp::os;

class yarp::os::Nodes::Private
{
public:
    Private();
    ~Private();

    void clear();
    Node* getNode(const std::string& name, bool create);
    void add(Contactable& contactable);
    void update(Contactable& contactable);
    void prepare(const std::string& name);
    void remove(Contactable& contactable);
    Contact query(const std::string& name, const std::string& category);
    void interrupt();
    bool enable(bool flag);
    Contact getParent(const std::string& name);
    Contact getURI(const std::string& name);
    void setActiveName(const std::string& name);
    std::string getActiveName();
    bool requireActiveName();
    void addExternalNode(const std::string& name, Node& node);
    void removeExternalNode(const std::string& name);

    // Port name
    // Pointer to Node
    // true = is external
    std::map<std::string, std::pair<Node*, bool>> nodes_map;

    std::mutex mutex;
    bool active{false};
    std::string active_name;
    Node* dummy{nullptr};
};

yarp::os::Nodes::Private::Private()
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
        if (n.second.first != nullptr) {
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

Node* yarp::os::Nodes::Private::getNode(const std::string& name, bool create)
{
    NestedContact nc(name);
    if (!nc.isNested()) {
        return nullptr;
    }
    Node* node = nullptr;
    auto it = nodes_map.find(nc.getNodeName());
    if (it != nodes_map.end()) {
        node = it->second.first;
    } else if (create) {
        mutex.lock();
        node = new Node();
        it = nodes_map.find(nc.getNodeName());
        if (it == nodes_map.end()) {
            nodes_map[nc.getNodeName()] = std::make_pair(node, false);
            node->prepare(nc.getNodeName());
        } else {
            // The node was created by some other thread while this
            // thread was waiting on the lock.
            delete node;
            node = it->second.first;
        }
        mutex.unlock();
    }
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
    if (node != nullptr) {
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
    if (node != nullptr) {
        node->update(contactable);
    }
}

void yarp::os::Nodes::Private::prepare(const std::string& name)
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
    if (node != nullptr) {
        node->remove(contactable);
    }
}

Contact yarp::os::Nodes::Private::query(const std::string& name, const std::string& category)
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

Contact yarp::os::Nodes::Private::getParent(const std::string& name)
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
Contact yarp::os::Nodes::Private::getURI(const std::string& name)
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

void yarp::os::Nodes::Private::setActiveName(const std::string& name)
{
    nodes_map[name].second = true;
    active_name = name;
}

std::string yarp::os::Nodes::Private::getActiveName()
{
    return active_name;
}

bool yarp::os::Nodes::Private::requireActiveName()
{
    if (active_name.empty()) {
        dummy = new Node("...");
    }
    return true;
}

void yarp::os::Nodes::Private::addExternalNode(const std::string& name, Node& node)
{
    mutex.lock();
    yAssert(nodes_map.find(name) == nodes_map.end());
    nodes_map[name] = std::make_pair(&node, true);
    mutex.unlock();
}

void yarp::os::Nodes::Private::removeExternalNode(const std::string& name)
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

Contact Nodes::query(const std::string& name, const std::string& category)
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

Contact Nodes::getParent(const std::string& name)
{
    return mPriv->getParent(name);
}

Contact Nodes::getURI(const std::string& name)
{
    return mPriv->getURI(name);
}

void Nodes::prepare(const std::string& name)
{
    mPriv->prepare(name);
}

void Nodes::update(Contactable& contactable)
{
    mPriv->update(contactable);
}

void Nodes::setActiveName(const std::string& name)
{
    mPriv->setActiveName(name);
}

std::string Nodes::getActiveName()
{
    return mPriv->getActiveName();
}

bool Nodes::requireActiveName()
{
    return mPriv->requireActiveName();
}

void Nodes::addExternalNode(const std::string& name, Node& node)
{
    mPriv->addExternalNode(name, node);
}

void Nodes::removeExternalNode(const std::string& name)
{
    mPriv->removeExternalNode(name);
}
