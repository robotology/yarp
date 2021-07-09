/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/RosNameSpace.h>

#include <yarp/conf/environment.h>

#include <yarp/os/DummyConnector.h>
#include <yarp/os/Os.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/PlatformLimits.h>

#include <cstdio>
#include <cstdlib>

using namespace yarp::os;
using namespace yarp::os::impl;

namespace {
YARP_OS_LOG_COMPONENT(ROSNAMESPACE, "yarp.os.RosNameSpace")
} // namespace


RosNameSpace::RosNameSpace(const Contact& contact) :
        mutex()
{
    this->contact = contact;
}

RosNameSpace::~RosNameSpace()
{
    stop();
}

Contact RosNameSpace::getNameServerContact() const
{
    return contact;
}

Contact RosNameSpace::queryName(const std::string& name)
{
    yCDebug(ROSNAMESPACE, "ROSNameSpace queryName(%s)", name.c_str());
    NestedContact nc(name);
    std::string node = nc.getNodeName();
    std::string srv = nc.getNestedName();
    std::string cat = nc.getCategory();
    bool is_service = false;

    Bottle cmd;
    Bottle reply;
    if (cat.find("-1") == std::string::npos) {
        cmd.addString("lookupNode");
        cmd.addString("dummy_id");
        cmd.addString(toRosNodeName(node));
        NetworkBase::write(getNameServerContact(),
                           cmd,
                           reply);
    }
    Contact contact;
    if (reply.get(0).asInt32() != 1) {
        cmd.clear();
        reply.clear();
        cmd.addString("lookupService");
        cmd.addString("dummy_id");
        cmd.addString(toRosNodeName(node));
        NetworkBase::write(getNameServerContact(),
                           cmd,
                           reply);
        is_service = true;
    }
    contact = Contact::fromString(reply.get(2).asString());
    // unfortunate differences in labeling carriers
    if (contact.getCarrier() == "rosrpc") {
        contact.setCarrier(std::string("rossrv+service.") + name);
    } else {
        contact.setCarrier("xmlrpc");
    }
    contact.setName(name);

    if (srv.empty() || !is_service) {
        return contact;
    }

    return Contact();
}

Contact RosNameSpace::registerName(const std::string& name)
{
    YARP_UNUSED(name);
    yCError(ROSNAMESPACE, "ROS name server does not do 'raw' registrations.");
    yCError(ROSNAMESPACE, "Use [Buffered]Port::open to get complete registrations.");
    std::exit(1);

    return Contact();
}

Contact RosNameSpace::registerContact(const Contact& contact)
{
    return registerAdvanced(contact, nullptr);
}

Contact RosNameSpace::registerAdvanced(const Contact& contact, NameStore* store)
{
    yCDebug(ROSNAMESPACE, "ROSNameSpace registerContact(%s / %s)",
               contact.toString().c_str(),
               contact.toURI().c_str());
    NestedContact nc = contact.getNested();
    if (nc.getNestedName().empty()) {
        nc.fromString(contact.getName());
    }
    std::string cat = nc.getCategory();
    if (!nc.getNestedName().empty()) {
        if (cat == "-1") {
            Bottle cmd;
            Bottle reply;
            cmd.clear();
            cmd.addString("registerService");
            cmd.addString(toRosNodeName(nc.getNodeName()));
            cmd.addString(toRosName(nc.getNestedName()));
            Contact rosrpc = contact;
            rosrpc.setCarrier("rosrpc");
            cmd.addString(rosrpc.toURI());
            Contact c;
            if (store != nullptr) {
                c = rosify(store->query(nc.getNodeName()));
            } else {
                Nodes& nodes = NameClient::getNameClient().getNodes();
                c = rosify(nodes.getParent(contact.getName()));
            }
            cmd.addString(c.toURI());
            bool ok = NetworkBase::write(getNameServerContact(),
                                         cmd,
                                         reply);
            if (!ok) {
                return Contact();
            }
        } else if (cat == "+" || cat == "-") {
            Bottle cmd;
            Bottle reply;
            cmd.clear();
            cmd.addString((cat == "+") ? "registerPublisher" : "registerSubscriber");
            cmd.addString(toRosNodeName(nc.getNodeName()));
            cmd.addString(toRosName(nc.getNestedName()));
            std::string typ = nc.getTypeNameStar();
            if (typ != "*" && !typ.empty()) {
                // remap some basic native YARP types
                if (typ == "yarp/image") {
                    typ = "sensor_msgs/Image";
                }
                if (typ.find('/') == std::string::npos) {
                    typ = std::string("yarp/") + typ;
                }
            }
            cmd.addString(typ);
            Contact c;
            if (store != nullptr) {
                c = rosify(store->query(nc.getNodeName()));
            } else {
                Nodes& nodes = NameClient::getNameClient().getNodes();
                c = rosify(nodes.getParent(contact.getName()));
            }
            //Contact c = rosify(contact);
            cmd.addString(c.toURI());
            bool ok = NetworkBase::write(getNameServerContact(),
                                         cmd,
                                         reply);
            if (!ok) {
                yCError(ROSNAMESPACE, "ROS registration error: %s", reply.toString().c_str());
                return Contact();
            }
            if (cat == "-") {
                Bottle* publishers = reply.get(2).asList();
                if ((publishers != nullptr) && publishers->size() >= 1) {
                    cmd.clear();
                    cmd.addString(contact.toURI());
                    cmd.addString("publisherUpdate");
                    cmd.addString("/yarp/RosNameSpace");
                    cmd.addString(toRosName(nc.getNestedName()));
                    cmd.addList() = *publishers;

                    mutex.lock();
                    bool need_start = false;
                    if (pending.size() == 0) {
                        mutex.unlock();
                        stop();
                        need_start = true;
                        mutex.lock();
                    }
                    pending.addList() = cmd;
                    if (need_start) {
                        start();
                    }
                    mutex.unlock();
                }
            }
        }
        return contact;
    }

    // Remainder of method is supporting older /name+#/foo syntax

    std::string name = contact.getName();
    size_t pub_idx = name.find("+#");
    size_t sub_idx = name.find("-#");

    std::string node;
    std::string pub;
    std::string sub;
    if (pub_idx != std::string::npos) {
        node = name.substr(0, pub_idx);
        pub = name.substr(pub_idx + 2, name.length());
        yCDebug(ROSNAMESPACE, "Publish to %s", pub.c_str());
    }
    if (sub_idx != std::string::npos) {
        node = name.substr(0, sub_idx);
        sub = name.substr(sub_idx + 2, name.length());
        yCDebug(ROSNAMESPACE, "Subscribe to %s", sub.c_str());
    }
    if (node.empty()) {
        node = name;
    }
    yCDebug(ROSNAMESPACE, "Name [%s] Node [%s] sub [%s] pub [%s]", name.c_str(), node.c_str(), sub.c_str(), pub.c_str());

    {
        Bottle cmd;
        Bottle reply;
        // for ROS, we fake port name registrations by
        // registering them as nodes that publish to an arbitrary
        // topic
        cmd.clear();
        cmd.addString("registerPublisher");
        cmd.addString(toRosNodeName(node));
        cmd.addString("/yarp/registration");
        cmd.addString("*");
        Contact c = rosify(contact);
        cmd.addString(c.toString());
        bool ok = NetworkBase::write(getNameServerContact(),
                                     cmd,
                                     reply);
        if (!ok) {
            return Contact();
        }
    }

    if (!pub.empty()) {
        NetworkBase::connect(node, std::string("topic:/") + pub);
    }
    if (!sub.empty()) {
        NetworkBase::connect(std::string("topic:/") + sub, node);
    }

    Contact c = contact;
    c.setName(node);
    return c;
}

Contact RosNameSpace::unregisterName(const std::string& name)
{
    return unregisterAdvanced(name, nullptr);
}

Contact RosNameSpace::unregisterAdvanced(const std::string& name, NameStore* store)
{
    NestedContact nc;
    nc.fromString(name);
    std::string cat = nc.getCategory();

    if (!nc.getNestedName().empty()) {
        if (cat == "-1") {
            Nodes& nodes = NameClient::getNameClient().getNodes();
            Contact c = nodes.getURI(name);
            c.setCarrier("rosrpc");
            c = rosify(c);
            Bottle cmd;
            Bottle reply;
            cmd.clear();
            cmd.addString("unregisterService");
            cmd.addString(toRosNodeName(nc.getNodeName()));
            cmd.addString(nc.getNestedName());
            cmd.addString(c.toURI());
            bool ok = NetworkBase::write(getNameServerContact(),
                                         cmd,
                                         reply);
            if (!ok) {
                return Contact();
            }
        } else if (cat == "+" || cat == "-") {
            Bottle cmd;
            Bottle reply;
            cmd.clear();
            cmd.addString((cat == "+") ? "unregisterPublisher" : "unregisterSubscriber");
            cmd.addString(toRosNodeName(nc.getNodeName()));
            cmd.addString(nc.getNestedName());
            Contact c;
            if (store != nullptr) {
                c = rosify(store->query(nc.getNodeName()));
            } else {
                Nodes& nodes = NameClient::getNameClient().getNodes();
                c = rosify(nodes.getParent(name));
            }
            cmd.addString(c.toString());
            bool ok = NetworkBase::write(getNameServerContact(),
                                         cmd,
                                         reply);
            if (!ok) {
                return Contact();
            }
        }
        return Contact();
    }

    // Remainder of method is supporting older /name+#/foo syntax

    size_t pub_idx = name.find("+#");
    size_t sub_idx = name.find("-#");

    std::string node;
    std::string pub;
    std::string sub;
    if (pub_idx != std::string::npos) {
        node = name.substr(0, pub_idx);
        pub = name.substr(pub_idx + 2, name.length());
    }
    if (sub_idx != std::string::npos) {
        node = name.substr(0, sub_idx);
        sub = name.substr(sub_idx + 2, name.length());
    }
    if (node.empty()) {
        node = name;
    }
    yCDebug(ROSNAMESPACE, "Name [%s] sub [%s] pub [%s]", name.c_str(), sub.c_str(), pub.c_str());

    if (!pub.empty()) {
        NetworkBase::disconnect(name, std::string("topic:/") + pub);
    }
    if (!sub.empty()) {
        NetworkBase::disconnect(std::string("topic:/") + sub, name);
    }

    Contact contact = NetworkBase::queryName(name);
    Bottle cmd;
    Bottle reply;
    cmd.addString("unregisterPublisher");
    cmd.addString(name);
    cmd.addString("/yarp/registration");
    Contact c("http", contact.getHost(), contact.getPort());
    cmd.addString(c.toString());
    bool ok = NetworkBase::write(getNameServerContact(),
                                 cmd,
                                 reply);
    if (!ok) {
        return Contact();
    }

    return Contact();
}

Contact RosNameSpace::unregisterContact(const Contact& contact)
{
    // Remainder of method is supporting older /name+#/foo syntax

    Bottle cmd;
    Bottle reply;
    cmd.addString("unregisterSubscriber");
    cmd.addString(contact.getName());
    cmd.addString("/yarp/registration");
    Contact c("http", contact.getHost(), contact.getPort());
    cmd.addString(c.toString());
    bool ok = NetworkBase::write(getNameServerContact(),
                                 cmd,
                                 reply);
    if (!ok) {
        return Contact();
    }
    return Contact();
}

bool RosNameSpace::setProperty(const std::string& name,
                               const std::string& key,
                               const Value& value)
{
    YARP_UNUSED(name);
    YARP_UNUSED(key);
    YARP_UNUSED(value);
    return false;
}

Value* RosNameSpace::getProperty(const std::string& name,
                                 const std::string& key)
{
    YARP_UNUSED(name);
    YARP_UNUSED(key);
    return nullptr;
}

bool RosNameSpace::connectPortToTopic(const Contact& src,
                                      const Contact& dest,
                                      const ContactStyle& style)
{
    Bottle cmd;
    cmd.addString("registerPublisher");
    cmd.addString(toRosNodeName(src.getName()));
    cmd.addString(dest.getName());
    cmd.addString("*");
    cmd.addString(rosify(src).toString());

    return connectTopic(cmd, false, src, dest, style, false);
}

bool RosNameSpace::connectTopicToPort(const Contact& src,
                                      const Contact& dest,
                                      const ContactStyle& style)
{
    Bottle cmd;
    cmd.addString("registerSubscriber");
    cmd.addString(toRosNodeName(dest.getName()));
    cmd.addString(src.getName());
    cmd.addString("*");
    cmd.addString(rosify(dest).toString());

    return connectTopic(cmd, true, src, dest, style, true);
}

bool RosNameSpace::disconnectPortFromTopic(const Contact& src,
                                           const Contact& dest,
                                           const ContactStyle& style)
{
    Bottle cmd;
    cmd.addString("unregisterPublisher");
    cmd.addString(toRosNodeName(src.getName()));
    cmd.addString(dest.getName());
    cmd.addString(rosify(src).toString());
    return connectTopic(cmd, false, src, dest, style, false);
}

bool RosNameSpace::disconnectTopicFromPort(const Contact& src,
                                           const Contact& dest,
                                           const ContactStyle& style)
{
    Bottle cmd;
    cmd.addString("unregisterSubscriber");
    cmd.addString(toRosNodeName(dest.getName()));
    cmd.addString(src.getName());
    cmd.addString(rosify(dest).toString());
    return connectTopic(cmd, true, src, dest, style, false);
}

bool RosNameSpace::connectPortToPortPersistently(const Contact& src,
                                                 const Contact& dest,
                                                 const ContactStyle& style)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    YARP_UNUSED(style);
    return false;
}

bool RosNameSpace::disconnectPortToPortPersistently(const Contact& src,
                                                    const Contact& dest,
                                                    const ContactStyle& style)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    YARP_UNUSED(style);
    return false;
}

bool RosNameSpace::connectTopic(Bottle& cmd,
                                bool srcIsTopic,
                                const Contact& src,
                                const Contact& dest,
                                const ContactStyle& style,
                                bool activeRegistration)
{
    Bottle reply;
    Contact dynamicSrc = src;
    Contact dynamicDest = dest;
    if (!style.carrier.empty()) {
        if (srcIsTopic) {
            dynamicDest.setCarrier(style.carrier);
        } else {
            dynamicSrc.setCarrier(style.carrier);
        }
    }
    Contact base = getNameServerContact();
    bool ok = NetworkBase::write(base,
                                 cmd,
                                 reply);
    bool fail = (reply.check("faultCode", Value(0)).asInt32() != 0) || !ok;
    if (fail) {
        if (!style.quiet) {
            yCError(ROSNAMESPACE, "Failure: name server did not accept connection to topic.");
            if (reply.check("faultString")) {
                yCError(ROSNAMESPACE, "Cause: %s", reply.check("faultString", Value("")).asString().c_str());
            }
        }
    }
    if (!fail) {
        if (activeRegistration) {
            Bottle* lst = reply.get(2).asList();
            Bottle cmd2;
            if (lst != nullptr) {
                cmd2.addString("publisherUpdate");
                cmd2.addString("/yarp");
                cmd2.addString(dynamicSrc.getName());
                cmd2.addList() = *lst;
                NetworkBase::write(dynamicDest,
                                   cmd2,
                                   reply,
                                   true);
            }
        }
    }
    return !fail;
}

bool RosNameSpace::localOnly() const
{
    return false;
}

bool RosNameSpace::usesCentralServer() const
{
    return true;
}

bool RosNameSpace::serverAllocatesPortNumbers() const
{
    return false;
}

bool RosNameSpace::connectionHasNameOfEndpoints() const
{
    return false;
}

Contact RosNameSpace::detectNameServer(bool useDetectedServer,
                                       bool& scanNeeded,
                                       bool& serverUsed)
{
    YARP_UNUSED(useDetectedServer);
    NameConfig nc;
    nc.fromFile();
    Contact c = nc.getAddress();
    scanNeeded = false;
    serverUsed = false;

    if (!c.isValid()) {
        scanNeeded = true;
        yCInfo(ROSNAMESPACE, "Checking for ROS_MASTER_URI...");
        std::string addr = yarp::conf::environment::get_string("ROS_MASTER_URI");
        c = Contact::fromString(addr);
        if (c.isValid()) {
            c.setCarrier("xmlrpc");
            c.setName(nc.getNamespace());
            NameConfig nc;
            nc.setAddress(c);
            nc.setMode("ros");
            nc.toFile();
            serverUsed = true;
        }
    }
    return c;
}

bool RosNameSpace::writeToNameServer(PortWriter& cmd,
                                     PortReader& reply,
                                     const ContactStyle& style)
{
    DummyConnector con0;
    cmd.write(con0.getWriter());
    Bottle in;
    in.read(con0.getReader());
    std::string key = in.get(0).asString();
    std::string arg1 = in.get(1).asString();

    Bottle cmd2;
    Bottle cache;

    if (key == "query") {
        Contact c = queryName(arg1);
        c.setName("");
        Bottle reply2;
        reply2.addString(arg1);
        reply2.addString(c.toString());
        DummyConnector con;
        reply2.write(con.getWriter());
        reply.read(con.getReader());
        return true;
    }

    if (key == "list") {
        cmd2.addString("getSystemState");
        cmd2.addString("dummy_id");

        if (!NetworkBase::write(getNameServerContact(), cmd2, cache, style)) {
            yCError(ROSNAMESPACE, "Failed to contact ROS server");
            return false;
        }

        Bottle out;
        out.addVocab32("many");
        Bottle* parts = cache.get(2).asList();
        Property nodes;
        Property topics;
        Property services;
        if (parts != nullptr) {
            for (int i = 0; i < 3; i++) {
                Bottle* part = parts->get(i).asList();
                if (part == nullptr) {
                    continue;
                }
                for (size_t j = 0; j < part->size(); j++) {
                    Bottle* unit = part->get(j).asList();
                    if (unit == nullptr) {
                        continue;
                    }
                    std::string stem = unit->get(0).asString();
                    Bottle* links = unit->get(1).asList();
                    if (links == nullptr) {
                        continue;
                    }
                    if (i < 2) {
                        topics.put(stem, 1);
                    } else {
                        services.put(stem, 1);
                    }
                    for (size_t j = 0; j < links->size(); j++) {
                        nodes.put(links->get(j).asString(), 1);
                    }
                }
            }
            Property* props[3] = {&nodes, &topics, &services};
            const char* title[3] = {"node", "topic", "service"};
            for (int p = 0; p < 3; p++) {
                Bottle blist;
                blist.read(*props[p]);
                for (size_t i = 0; i < blist.size(); i++) {
                    std::string name = blist.get(i).asList()->get(0).asString();
                    Bottle& info = out.addList();
                    info.addString(title[p]);
                    info.addString(name);
                }
            }
        }
        out.write(reply);
        return true;
    }

    return false;
}


std::string RosNameSpace::toRosName(const std::string& name)
{
    if (name.find(':') == std::string::npos) {
        return name;
    }
    std::string result;
    for (char i : name) {
        if (i != ':') {
            result += i;
        } else {
            result += "__";
        }
    }
    return result;
}

std::string RosNameSpace::fromRosName(const std::string& name)
{
    if (name.find("__") == std::string::npos) {
        return name;
    }
    // length is at least 2
    std::string result;
    int ct = 0;
    for (char i : name) {
        if (i != '_') {
            if (ct != 0) {
                result += '_';
            }
            result += i;
            ct = 0;
        } else {
            ct++;
            if (ct == 2) {
                result += ':';
                ct = 0;
            }
        }
    }
    if (ct != 0) {
        result += '_';
    }
    return result;
}

std::string RosNameSpace::toRosNodeName(const std::string& name)
{
    return toRosName(name);
}

std::string RosNameSpace::fromRosNodeName(const std::string& name)
{
    return fromRosName(name);
}

Contact RosNameSpace::rosify(const Contact& contact)
{
    std::string carrier = ((contact.getCarrier() == "rosrpc") ? "rosrpc" : "http");
    std::string hostname = contact.getHost();
    if (yarp::os::impl::NameConfig::isLocalName(hostname)) {
        char hn[HOST_NAME_MAX];
        yarp::os::gethostname(hn, sizeof(hn));
        hostname = hn;
    }
    return Contact(carrier, hostname, contact.getPort());
}


void RosNameSpace::run()
{
    int pct = 0;
    do {
        mutex.lock();
        pct = pending.size();
        mutex.unlock();
        if (pct > 0) {
            mutex.lock();
            Bottle* bot = pending.get(0).asList();
            Bottle curr = *bot;
            mutex.unlock();

            yCDebug(ROSNAMESPACE, "ROS connection begins: %s", curr.toString().c_str());
            ContactStyle style;
            style.admin = true;
            style.carrier = "tcp";
            Bottle cmd = curr.tail();
            Contact contact = Contact::fromString(curr.get(0).asString());
            contact.setName("");
            Bottle reply;
            NetworkBase::write(contact, cmd, reply, style);
            yCDebug(ROSNAMESPACE, "ROS connection ends: %s", curr.toString().c_str());

            mutex.lock();
            pending = pending.tail();
            pct = pending.size();
            mutex.unlock();
        }
    } while (pct > 0);
}
