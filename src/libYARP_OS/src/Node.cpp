/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick, Tobias Fischer
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/conf/compiler.h>
#include <yarp/os/Node.h>
#include <yarp/os/Mutex.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/NestedContact.h>
#include <yarp/os/Port.h>
#include <yarp/os/PortReport.h>
#include <yarp/os/PortInfo.h>
#include <yarp/os/Network.h>
#include <yarp/os/RosNameSpace.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/NameClient.h>

#include <algorithm>
#include <vector>
#include <list>
#include <map>

using namespace yarp::os;
using namespace yarp::os::impl;

class ROSReport : public PortReport {
public:
    std::multimap<std::string, std::string> outgoingURIs;
    std::multimap<std::string, std::string> incomingURIs;

    ROSReport() {
    }

    virtual void report(const PortInfo& info) {
        if (info.tag == PortInfo::PORTINFO_CONNECTION) {
            NameClient& nic = NameClient::getNameClient();
            Contact c;
            if (info.incoming) {
                c = RosNameSpace::rosify(nic.queryName(info.sourceName));

            } else {
                c = RosNameSpace::rosify(nic.queryName(info.targetName));
            }

            if (info.incoming) {
                incomingURIs.insert(std::make_pair(info.portName, c.toURI()));
            } else {
                outgoingURIs.insert(std::make_pair(info.portName, c.toURI()));
            }
        }
    }
};

static ConstString toRosName(const ConstString& str) {
    return RosNameSpace::toRosName(str);
}

static ConstString fromRosName(const ConstString& str)
{
    return RosNameSpace::fromRosName(str);
}

class NodeItem
{
public:
    NestedContact nc;
    Contactable *contactable;

    void update()
    {
        if (nc.getTypeName()=="") {
            if (!contactable) {
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
        ConstString cat = nc.getCategory();
        return (cat=="" || cat=="-");
    }

    bool isPublisher()
    {
        ConstString cat = nc.getCategory();
        return (cat=="" || cat=="+");
    }

    bool isTopic()
    {
        ConstString cat = nc.getCategory();
        return (cat=="" || cat=="+" || cat=="-");
    }

    bool isServiceServer()
    {
        ConstString cat = nc.getCategory();
        return (cat=="" || cat=="-1");
    }

    bool isServiceClient()
    {
        ConstString cat = nc.getCategory();
        return (cat=="" || cat=="+1");
    }

    bool isService()
    {
        ConstString cat = nc.getCategory();
        return (cat=="" || cat=="+1" || cat=="-1");
    }
};

class NodeArgs
{
public:
    Bottle request;
    Bottle args;
    Value reply;
    int code;
    ConstString msg;
    bool should_drop;

    NodeArgs()
    {
        code = -1;
        should_drop = true;
    }

    void error(const char *txt)
    {
        msg = txt;
        code = -1;
    }

    void fail(const char *txt)
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
        code = alt.get(0).asInt();
        msg = alt.get(1).asString();
        reply = alt.get(2);
    }
};

class yarp::os::Node::Helper : public PortReader
{
public:
    std::multimap<ConstString,NodeItem> by_part_name;
    std::multimap<ConstString,NodeItem> by_category;
    std::map<Contactable*,NodeItem> name_cache;
    Port port;
    Node *owner;

    Mutex mutex;
    ConstString name;
    ConstString prev_name;
    bool has_prev_name;

    Helper() :
            owner(YARP_NULLPTR),
            prev_name(""),
            has_prev_name(false)
    {
        clear();
        port.includeNodeInName(false);
    }

    ~Helper()
    {
        clear();
        port.close();
    }

    void clear()
    {
        if (!mutex.tryLock()) {
            return;
        }
        while (name_cache.begin() != name_cache.end()) {
            Contactable *c = name_cache.begin()->first;
            if (c) {
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
    std::vector<Contact> query(const ConstString& name, const ConstString& category = ConstString());

    void prepare(const ConstString& name);

    void interrupt()
    {
        port.interrupt();
    }

    virtual bool read(ConnectionReader& reader);

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
        for (std::multimap<ConstString,NodeItem>::iterator it = by_part_name.begin(); it != by_part_name.end(); it++) {
            NodeItem& item = it->second;
            if (!(item.isSubscriber() || item.isPublisher())) {
                continue;
            }
            item.update();
            item.contactable->getReport(report);
        }
        mutex.unlock();

        for (std::multimap<std::string, std::string>::const_iterator it = report.outgoingURIs.begin(); it != report.outgoingURIs.end(); ++it) {
            Bottle& lst = connections->addList();
            lst.addInt(opaque_id); // connectionId
            lst.addString(it->second);
            lst.addString("o");
            lst.addString((it->second != "/yarp/node" ? "TCPROS" : "YARP"));
            NestedContact nc(it->first);
            lst.addString(toRosName(nc.getNestedName()));
            opaque_id++;
        }

        for (std::multimap<std::string, std::string>::const_iterator it = report.incomingURIs.begin(); it != report.incomingURIs.end(); ++it) {
            Bottle& lst = connections->addList();
            lst.addInt(opaque_id); // connectionId
            lst.addString(it->second);
            lst.addString("i");
            lst.addString((it->second != "/yarp/node" ? "TCPROS" : "YARP"));
            NestedContact nc(it->first);
            lst.addString(toRosName(nc.getNestedName()));
            opaque_id++;
        }

        na.reply = v;
        na.success();
    }

    void getMasterUri(NodeArgs& na)
    {
        na.reply = Value(NetworkBase::getEnvironment("ROS_MASTER_URI"));
        na.success();
    }

    void shutdown(NodeArgs& na)
    {
        na.reply = Value(ConstString("hmm"));
    }

    void getPid(NodeArgs& na)
    {
        na.reply = Value(static_cast<int>(ACE_OS::getpid()));
        na.success();
    }

    void getSubscriptions(NodeArgs& na)
    {
        Value v;
        Bottle* subscriptions = v.asList();
        mutex.lock();
        for (std::multimap<ConstString,NodeItem>::iterator it = by_part_name.begin(); it != by_part_name.end(); it++) {
            NodeItem& item = it->second;
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
        for (std::multimap<ConstString,NodeItem>::iterator it = by_part_name.begin(); it != by_part_name.end(); it++) {
            NodeItem& item = it->second;
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
        na.reply = Value(ConstString("hmm"));
    }

    void publisherUpdate(NodeArgs& na)
    {
        ConstString topic = fromRosName(na.args.get(0).asString());
        std::vector<Contact> contacts = query(topic, "-");
        if (contacts.size() < 1) {
            na.fail("Cannot find topic");
            return;
        }

        for (std::vector<Contact>::iterator it = contacts.begin(); it != contacts.end(); ++it) {
            Contact &c = *it;
            if (!c.isValid()) {
                continue;
            }
            c.setName("");
            // just pass the message along, YARP ports know what to do with it
            ContactStyle style;
            style.admin = true;
            style.carrier = "tcp";
            Bottle reply;
            if (!NetworkBase::write(c,na.request,reply,style)) {
                continue;
            }
            na.fromExternal(reply);
        }
    }

    void requestTopic(NodeArgs& na)
    {
        ConstString topic = na.args.get(0).asString();
        topic = fromRosName(topic);
        std::vector<Contact> contacts = query(topic, "+");
        if (contacts.size() < 1) {
            na.fail("Cannot find topic");
            return;
        }
        for (std::vector<Contact>::iterator it = contacts.begin(); it != contacts.end(); ++it) {
            Contact &c = *it;

            if (!c.isValid()) {
                continue;
            }
            Value v;
            Bottle* lst = v.asList();
            lst->addString("TCPROS");
            lst->addString(c.getHost());
            lst->addInt(c.getPort());
            na.reply = v;
            na.success();
            return;
        }
        na.fail("Cannot find topic");
    }
};

void yarp::os::Node::Helper::prepare(const ConstString& name)
{
    if (port.getName()=="") {
        port.setReader(*this);
        Property *prop = port.acquireProperties(false);
        if (prop) {
            prop->put("node_like",1);
        }
        port.releaseProperties(prop);
        port.open(name);
        this->name = port.getName();
    }
}

void yarp::os::Node::Helper::add(Contactable& contactable)
{
    NodeItem item;
    item.nc.fromString(contactable.getName());
    if (name=="") name = item.nc.getNodeName();
    if (name!=item.nc.getNodeName()) {
        fprintf(stderr,"Node name mismatch, expected [%s] but got [%s]\n",
                name.c_str(), item.nc.getNodeName().c_str());
        return;
    }
    prepare(name);
    item.contactable = &contactable;

    mutex.lock();
    name_cache[&contactable] = item;
    by_category.insert(std::pair<ConstString,NodeItem>(item.nc.getCategory(),item));
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
    by_part_name.erase(item.nc.getNestedName());
    by_category.erase(item.nc.getCategory());
    mutex.unlock();
}

std::vector<Contact> yarp::os::Node::Helper::query(const ConstString& name, const ConstString& category)
{
    std::vector<Contact> contacts;
    mutex.lock();
    for (std::multimap<ConstString,NodeItem>::const_iterator it = by_part_name.begin(); it != by_part_name.end(); ++it) {
        if (it->first == name && (category.empty() || category == it->second.nc.getCategory())) {
#if defined(YARP_HAS_CXX11)
                contacts.emplace_back(it->second.contactable->where());
#else
                contacts.push_back(it->second.contactable->where());
#endif
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
    //printf("NODE API for %s received %s\n",
    //name.c_str(),
    //na.request.toString().c_str());
    ConstString key = na.request.get(0).asString();
    na.args = na.request.tail().tail();
    if (key=="getBusStats") {
        getBusStats(na);
    } else if (key=="getBusInfo") {
        getBusInfo(na);
    } else if (key=="getMasterUri") {
        getMasterUri(na);
    } else if (key=="shutdown") {
        shutdown(na);
    } else if (key=="getPid") {
        getPid(na);
    } else if (key=="getSubscriptions") {
        getSubscriptions(na);
    } else if (key=="getPublications") {
        getPublications(na);
    } else if (key=="paramUpdate") {
        paramUpdate(na);
    } else if (key=="publisherUpdate") {
        publisherUpdate(na);
    } else if (key=="requestTopic") {
        requestTopic(na);
    } else {
        na.error("I have no idea what you are talking about");
    }
    if (na.should_drop) {
        reader.requestDrop(); // ROS likes to close down.
    }
    if (reader.getWriter()) {
        Bottle full;
        full.addInt(na.code);
        full.addString(na.msg);
        full.add(na.reply);
        //printf("NODE %s <<< %s\n",
        //name.c_str(),
        //full.toString().c_str());
        full.write(*reader.getWriter());
    }
    return true;
}



Node::Node() :
        mPriv(new Helper)
{
    yAssert(mPriv != YARP_NULLPTR);
    mPriv->owner = this;
}

Node::Node(const ConstString& name) :
        mPriv(new Helper)
{
    yAssert(mPriv != YARP_NULLPTR);
    mPriv->owner = this;
    Nodes& nodes = NameClient::getNameClient().getNodes();
    mPriv->prev_name = nodes.getActiveName();
    mPriv->has_prev_name = true;
    mPriv->name = name;
    prepare(name);
    ConstString rname = mPriv->port.getName();
    nodes.addExternalNode(rname,*this);
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

Contact Node::query(const ConstString& name, const ConstString& category)
{
    std::vector<Contact> contacts = mPriv->query(name,category);
    if (contacts.size() >= 1) {
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

void Node::prepare(const ConstString& name)
{
    mPriv->mutex.lock();
    mPriv->prepare(name);
    mPriv->mutex.unlock();
}
