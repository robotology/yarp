/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Node.h>

#include <yarp/conf/compiler.h>
#include <yarp/conf/environment.h>

#include <yarp/os/NestedContact.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/Port.h>
#include <yarp/os/PortInfo.h>
#include <yarp/os/PortReport.h>
#include <yarp/os/RosNameSpace.h>
#include <yarp/os/Type.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/NameClient.h>

#include <algorithm>
#include <cstdlib>
#include <list>
#include <map>
#include <vector>
#include <mutex>

using namespace yarp::os;
using namespace yarp::os::impl;

namespace {
YARP_OS_LOG_COMPONENT(NODE, "yarp.os.Node")
}


class ROSReport : public PortReport
{
public:
    std::multimap<std::string, std::string> outgoingURIs;
    std::multimap<std::string, std::string> incomingURIs;

    ROSReport() = default;

    void report(const PortInfo& info) override
    {
        if (info.tag == PortInfo::PORTINFO_CONNECTION) {
            NameClient& nic = NameClient::getNameClient();
            Contact c;
            if (info.incoming) {
                c = RosNameSpace::rosify(nic.queryName(info.sourceName));
                incomingURIs.insert(std::make_pair(info.portName, c.toURI()));
            } else {
                c = RosNameSpace::rosify(nic.queryName(info.targetName));
                outgoingURIs.insert(std::make_pair(info.portName, c.toURI()));
            }
        }
    }
};

static std::string toRosName(const std::string& str)
{
    return RosNameSpace::toRosName(str);
}

static std::string fromRosName(const std::string& str)
{
    return RosNameSpace::fromRosName(str);
}

class NodeItem
{
public:
    NestedContact nc;
    Contactable* contactable;

    void update()
    {
        if (nc.getTypeName().empty()) {
            if (contactable == nullptr) {
                return;
            }
            Type typ = contactable->getType();
            if (typ.isValid()) {
                nc.setTypeName(typ.getName());
            }
        }
    }

    bool isSubscriber()
    {
        std::string cat = nc.getCategory();
        return (cat.empty() || cat == "-");
    }

    bool isPublisher()
    {
        std::string cat = nc.getCategory();
        return (cat.empty() || cat == "+");
    }

    bool isTopic()
    {
        std::string cat = nc.getCategory();
        return (cat.empty() || cat == "+" || cat == "-");
    }

    bool isServiceServer()
    {
        std::string cat = nc.getCategory();
        return (cat.empty() || cat == "-1");
    }

    bool isServiceClient()
    {
        std::string cat = nc.getCategory();
        return (cat.empty() || cat == "+1");
    }

    bool isService()
    {
        std::string cat = nc.getCategory();
        return (cat.empty() || cat == "+1" || cat == "-1");
    }
};

class NodeArgs
{
public:
    Bottle request;
    Bottle args;
    Value reply;
    int code;
    std::string msg;
    bool should_drop;

    NodeArgs()
    {
        code = -1;
        should_drop = true;
    }

    void error(const char* txt)
    {
        msg = txt;
        code = -1;
    }

    void fail(const char* txt)
    {
        msg = txt;
        code = 0;
    }

    void success()
    {
        msg = "";
        code = 1;
    }

    void drop()
    {
        should_drop = true;
    }

    void persist()
    {
        should_drop = false;
    }

    void fromExternal(const Bottle& alt)
    {
        code = alt.get(0).asInt32();
        msg = alt.get(1).asString();
        reply = alt.get(2);
    }
};

class yarp::os::Node::Helper : public PortReader
{
public:
    std::multimap<std::string, NodeItem> by_part_name;
    std::multimap<std::string, NodeItem> by_category;
    std::map<Contactable*, NodeItem> name_cache;
    Port port;
    Node* owner{nullptr};

    std::mutex mutex;
    std::string name;
    std::string prev_name;
    bool has_prev_name{false};

    Helper()
    {
        clear();
        port.includeNodeInName(false);
    }

    ~Helper() override
    {
        clear();
        port.close();
    }

    void clear()
    {
        if (!mutex.try_lock()) {
            return;
        }
        while (name_cache.begin() != name_cache.end()) {
            Contactable* c = name_cache.begin()->first;
            if (c != nullptr) {
                mutex.unlock();
                c->interrupt();
                c->close();
                mutex.lock();
                // Close will remove the Contactable from the map only the first
                // time that a node is found (for example if "/foo+@/node" and
                // "/foo-@node" are registered, only the first time that "/node"
                // is found it is removed automatically. The second time it must
                // be removed manually.
                if (!name_cache.empty() && name_cache.begin()->first == c) {
                    name_cache.erase(name_cache.begin());
                }
            }
        }
        mutex.unlock();
        port.interrupt();
    }

    void add(Contactable& contactable);
    void update(Contactable& contactable);
    void remove(Contactable& contactable);
    std::vector<Contact> query(const std::string& name, const std::string& category = std::string());

    void prepare(const std::string& name);

    void interrupt()
    {
        port.interrupt();
    }

    bool read(ConnectionReader& reader) override;

    void getBusStats(NodeArgs& na)
    {
        na.reply = Value();
        na.success();
    }

    void getBusInfo(NodeArgs& na)
    {
        unsigned int opaque_id = 1;
        ROSReport report;
        Value v;
        Bottle* connections = v.asList();

        mutex.lock();
        for (auto& it : by_part_name) {
            NodeItem& item = it.second;
            if (!(item.isSubscriber() || item.isPublisher())) {
                continue;
            }
            item.update();
            item.contactable->getReport(report);
        }
        mutex.unlock();

        for (std::multimap<std::string, std::string>::const_iterator it = report.outgoingURIs.begin(); it != report.outgoingURIs.end(); ++it) {
            Bottle& lst = connections->addList();
            lst.addInt32(opaque_id); // connectionId
            lst.addString(it->second);
            lst.addString("o");
            lst.addString("TCPROS");
            NestedContact nc(it->first);
            lst.addString(toRosName(nc.getNestedName()));
            opaque_id++;
        }

        for (std::multimap<std::string, std::string>::const_iterator it = report.incomingURIs.begin(); it != report.incomingURIs.end(); ++it) {
            Bottle& lst = connections->addList();
            lst.addInt32(opaque_id); // connectionId
            lst.addString(it->second);
            lst.addString("i");
            lst.addString("TCPROS");
            NestedContact nc(it->first);
            lst.addString(toRosName(nc.getNestedName()));
            opaque_id++;
        }

        if (connections->size() == 0) {
            connections->addList(); // add empty list
        }

        na.reply = v;
        na.success();
    }

    void getMasterUri(NodeArgs& na)
    {
        na.reply = Value(yarp::conf::environment::get_string("ROS_MASTER_URI"));
        na.success();
    }

    void shutdown(NodeArgs& na)
    {
        na.reply = Value(std::string("hmm"));
    }

    void getPid(NodeArgs& na)
    {
        na.reply = Value(static_cast<int>(yarp::os::getpid()));
        na.success();
    }

    void getSubscriptions(NodeArgs& na)
    {
        Value v;
        Bottle* subscriptions = v.asList();
        mutex.lock();
        for (auto& it : by_part_name) {
            NodeItem& item = it.second;
            if (!item.isSubscriber()) {
                continue;
            }
            item.update();
            Bottle& lst = subscriptions->addList();
            lst.addString(toRosName(item.nc.getNestedName()));
            lst.addString(item.nc.getTypeName());
        }
        mutex.unlock();
        na.reply = v;
        na.success();
    }

    void getPublications(NodeArgs& na)
    {
        Value v;
        Bottle* publications = v.asList();
        mutex.lock();
        for (auto& it : by_part_name) {
            NodeItem& item = it.second;
            if (!item.isPublisher()) {
                continue;
            }
            item.update();
            Bottle& lst = publications->addList();
            lst.addString(toRosName(item.nc.getNestedName()));
            lst.addString(item.nc.getTypeName());
        }
        mutex.unlock();
        na.reply = v;
        na.success();
    }

    void paramUpdate(NodeArgs& na)
    {
        na.reply = Value(std::string("hmm"));
    }

    void publisherUpdate(NodeArgs& na)
    {
        std::string topic = fromRosName(na.args.get(0).asString());
        std::vector<Contact> contacts = query(topic, "-");
        if (contacts.empty()) {
            na.fail("Cannot find topic");
            return;
        }

        for (auto& c : contacts) {
            if (!c.isValid()) {
                continue;
            }
            c.setName("");
            // just pass the message along, YARP ports know what to do with it
            ContactStyle style;
            style.admin = true;
            style.carrier = "tcp";
            Bottle reply;
            if (!NetworkBase::write(c, na.request, reply, style)) {
                continue;
            }
            na.fromExternal(reply);
        }
    }

    void requestTopic(NodeArgs& na)
    {
        std::string topic = na.args.get(0).asString();
        topic = fromRosName(topic);
        std::vector<Contact> contacts = query(topic, "+");
        if (contacts.empty()) {
            na.fail("Cannot find topic");
            return;
        }
        for (auto& c : contacts) {
            if (!c.isValid()) {
                continue;
            }
            Value v;
            Bottle* lst = v.asList();
            lst->addString("TCPROS");
            lst->addString(c.getHost());
            lst->addInt32(c.getPort());
            na.reply = v;
            na.success();
            return;
        }
        na.fail("Cannot find topic");
    }
};

void yarp::os::Node::Helper::prepare(const std::string& name)
{
    mutex.lock();
    if (port.getName().empty()) {
        port.setReader(*this);
        Property* prop = port.acquireProperties(false);
        if (prop != nullptr) {
            prop->put("node_like", 1);
        }
        port.releaseProperties(prop);
        port.open(name);
        this->name = port.getName();
    }
    mutex.unlock();
}

void yarp::os::Node::Helper::add(Contactable& contactable)
{
    NodeItem item;
    item.nc.fromString(contactable.getName());
    if (name.empty()) {
        name = item.nc.getNodeName();
    }
    if (name != item.nc.getNodeName()) {
        yCError(NODE, "Node name mismatch, expected [%s] but got [%s]\n", name.c_str(), item.nc.getNodeName().c_str());
        return;
    }
    prepare(name);
    item.contactable = &contactable;

    mutex.lock();
    name_cache[&contactable] = item;
    by_part_name.insert(std::pair<std::string, NodeItem>(item.nc.getNestedName(), item));
    by_category.insert(std::pair<std::string, NodeItem>(item.nc.getCategory(), item));
    mutex.unlock();
}

void yarp::os::Node::Helper::update(Contactable& contactable)
{
    mutex.lock();
    NodeItem item = name_cache[&contactable];
    mutex.unlock();
}

void yarp::os::Node::Helper::remove(Contactable& contactable)
{
    mutex.lock();
    NodeItem item = name_cache[&contactable];
    name_cache.erase(&contactable);
    std::string nestedName = item.nc.getNestedName();
    for (auto it = by_part_name.begin(); it != by_part_name.end(); ++it) {
        if (it->first == nestedName && it->second.contactable->where().toString() == contactable.where().toString()) {
            by_part_name.erase(it);
            break;
        }
    }
    std::string category = item.nc.getCategory();
    for (auto it = by_category.begin(); it != by_category.end(); ++it) {
        if (it->first == category && it->second.contactable->where().toString() == contactable.where().toString()) {
            by_category.erase(it);
            break;
        }
    }
    mutex.unlock();
}

std::vector<Contact> yarp::os::Node::Helper::query(const std::string& name, const std::string& category)
{
    std::vector<Contact> contacts;
    mutex.lock();
    for (std::multimap<std::string, NodeItem>::const_iterator it = by_part_name.begin(); it != by_part_name.end(); ++it) {
        if (it->first == name && (category.empty() || category == it->second.nc.getCategory())) {
            contacts.emplace_back(it->second.contactable->where());
        }
    }
    mutex.unlock();

    return contacts;
}

bool yarp::os::Node::Helper::read(ConnectionReader& reader)
{
    if (!reader.isValid()) {
        return false;
    }
    NodeArgs na;
    na.request.read(reader);
    yCDebug(NODE, "NODE API for %s received %s\n", name.c_str(), na.request.toString().c_str());
    std::string key = na.request.get(0).asString();
    na.args = na.request.tail().tail();
    if (key == "getBusStats") {
        getBusStats(na);
    } else if (key == "getBusInfo") {
        getBusInfo(na);
    } else if (key == "getMasterUri") {
        getMasterUri(na);
    } else if (key == "shutdown") {
        shutdown(na);
    } else if (key == "getPid") {
        getPid(na);
    } else if (key == "getSubscriptions") {
        getSubscriptions(na);
    } else if (key == "getPublications") {
        getPublications(na);
    } else if (key == "paramUpdate") {
        paramUpdate(na);
    } else if (key == "publisherUpdate") {
        publisherUpdate(na);
    } else if (key == "requestTopic") {
        requestTopic(na);
    } else {
        na.error("I have no idea what you are talking about");
    }
    if (na.should_drop) {
        reader.requestDrop(); // ROS likes to close down.
    }
    if (reader.getWriter() != nullptr) {
        Bottle full;
        full.addInt32(na.code);
        full.addString(na.msg);
        full.add(na.reply);
        yCDebug(NODE, "NODE %s <<< %s\n", name.c_str(), full.toString().c_str());
        full.write(*reader.getWriter());
    }
    return true;
}


Node::Node() :
        mPriv(new Helper)
{
    yCAssert(NODE, mPriv != nullptr);
    mPriv->owner = this;
}

Node::Node(const std::string& name) :
        mPriv(new Helper)
{
    yCAssert(NODE, mPriv != nullptr);
    mPriv->owner = this;
    Nodes& nodes = NameClient::getNameClient().getNodes();
    mPriv->prev_name = nodes.getActiveName();
    mPriv->has_prev_name = true;
    mPriv->name = name;
    prepare(name);
    std::string rname = mPriv->port.getName();
    nodes.addExternalNode(rname, *this);
    nodes.setActiveName(rname);
}

Node::~Node()
{
    if (mPriv->has_prev_name) {
        Nodes& nodes = NameClient::getNameClient().getNodes();
        nodes.setActiveName(mPriv->prev_name);
        nodes.removeExternalNode(mPriv->name);
    }
    delete mPriv;
}

void Node::add(Contactable& contactable)
{
    mPriv->add(contactable);
}

void Node::update(Contactable& contactable)
{
    mPriv->update(contactable);
}

void Node::remove(Contactable& contactable)
{
    mPriv->remove(contactable);
}

Contact Node::query(const std::string& name, const std::string& category)
{
    std::vector<Contact> contacts = mPriv->query(name, category);
    if (!contacts.empty()) {
        return contacts.at(0);
    }
    return Contact();
}

void Node::interrupt()
{
    mPriv->interrupt();
}

Contact Node::where()
{
    return mPriv->port.where();
}

void Node::prepare(const std::string& name)
{
    mPriv->prepare(name);
}
