/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/MultiNameSpace.h>

#include <yarp/os/RosNameSpace.h>
#include <yarp/os/Time.h>
#include <yarp/os/YarpNameSpace.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/NameConfig.h>

#include <vector>

using namespace yarp::os;
using namespace yarp::os::impl;

namespace {
YARP_OS_LOG_COMPONENT(MULTINAMESPACE, "yarp.os.MultiNamespace" )
} // namespace

using SpaceList = std::vector<NameSpace*>;

// private implementation of a namespace container
class MultiNameSpaceHelper
{
public:
    SpaceList spaces; // list of all namespaces

    // a cache for common flags once we compute them
    bool _localOnly;
    bool _usesCentralServer;
    bool _serverAllocatesPortNumbers;
    bool _connectionHasNameOfEndpoints;

    MultiNameSpaceHelper()
    {
        clear();
    }

    ~MultiNameSpaceHelper()
    {
        clear();
    }

    void clear()
    {
        // remove all namespaces and reset flags
        for (auto ns : spaces) {
            if (ns != nullptr) {
                delete ns;
                ns = nullptr;
            }
        }
        spaces.clear();
        _localOnly = true;
        _usesCentralServer = false;
        _serverAllocatesPortNumbers = false;
        _connectionHasNameOfEndpoints = true;
    }

    void scan()
    {
        // reset flags
        _localOnly = true;
        _usesCentralServer = false;
        _serverAllocatesPortNumbers = true;
        // now scan each namespace
        for (auto ns : spaces) {
            if (ns == nullptr) {
                continue;
            }
            // if any namespace is nonlocal, combination is nonlocal
            if (!ns->localOnly()) {
                _localOnly = false;
            }
            // if any namespace uses a central server, combination also does
            if (ns->usesCentralServer()) {
                _usesCentralServer = true;
            }
            // if any namespace doesn't allocate port numbers, combination
            // cannot be relied on to do so either
            if (!ns->serverAllocatesPortNumbers()) {
                _serverAllocatesPortNumbers = false;
            }
            // if any namespace lacks informed connections, combination
            // cannot be relied on to be informed either
            if (!ns->connectionHasNameOfEndpoints()) {
                _connectionHasNameOfEndpoints = false;
            }
        }
    }

    bool setLocalMode(bool flag)
    {
        // remove any existing namespaces
        clear();
        if (flag) {
            // add a dummy local namespace
            NameSpace* ns = new YarpDummyNameSpace;
            spaces.push_back(ns);
        }
        // cache flags
        scan();
        return true;
    }

    bool activate(bool force = false)
    {
        if (force) {
            // wipe if forced
            clear();
        }
        // return if namespaces already present
        if (!spaces.empty()) {
            return true;
        }
        // read namespace list from config file
        NameConfig conf;
        if (!conf.fromFile()) {
            double now = SystemClock::nowSystem();
            static double last_shown = now - 10;
            if (now - last_shown > 3) {
                last_shown = now;
                yCWarning(MULTINAMESPACE, "YARP name server(s) not configured, ports will be anonymous\n");
                yCWarning(MULTINAMESPACE, "check your namespace and settings with 'yarp detect'\n");
            }
            return false;
        }
        Bottle ns = conf.getNamespaces();
        // loop through namespaces
        for (size_t i = 0; i < ns.size(); i++) {
            std::string n = ns.get(i).asString();
            NameConfig conf2;
            // read configuration of individual namespace
            if (!conf2.fromFile(n.c_str())) {
                yCWarning(MULTINAMESPACE, "Could not find namespace %s\n", n.c_str());
                continue;
            }
            std::string mode = conf2.getMode();
            Contact address = conf2.getAddress();
            address.setName(n);
            if (mode == "yarp" || mode == "//") {
                // add a yarp namespace
                NameSpace* ns = new YarpNameSpace(address);
                spaces.push_back(ns);
            } else if (mode == "ros") {
                // add a ros namespace
                NameSpace* ns = new RosNameSpace(address);
                spaces.push_back(ns);
            } else if (mode == "local") {
                NameSpace* ns = new YarpDummyNameSpace;
                spaces.push_back(ns);
            } else {
                // shrug
                yCError(MULTINAMESPACE, "cannot deal with namespace of type %s", mode.c_str());
                return false;
            }
        }
        // cache flags
        scan();
        return true;
    }

    Contact getNameServerContact()
    {
        activate(); // make sure we've loaded namespace(s)
        if (!spaces.empty()) {
            // return first name server
            return spaces[0]->getNameServerContact();
        }
        return Contact();
    }

    Contact queryName(const std::string& name)
    {
        activate();
        // try query against each namespace in order
        for (auto ns : spaces) {
            if (ns == nullptr) {
                continue;
            }
            if (ns->getNameServerName() == name) {
                // optimization: return cached server address for
                // port names that match name of namespace
                return ns->getNameServerContact();
            }
            Contact result = ns->queryName(name);
            // return a result once we get one, skipping any remaining
            // namespaces
            if (result.isValid()) {
                return result;
            }
        }
        return Contact();
    }

    // return one namespace, any namespace (in fact always first)
    NameSpace* getOne()
    {
        activate();
        if (spaces.empty()) {
            return nullptr;
        }
        return spaces[0];
    }

    // return full list of namespaces
    SpaceList& getAll()
    {
        activate();
        return spaces;
    }
};

#define HELPER(x) (*((MultiNameSpaceHelper*)((x)->system_resource)))

MultiNameSpace::MultiNameSpace()
{
    altStore = nullptr;
    system_resource = new MultiNameSpaceHelper;
    yCAssert(MULTINAMESPACE, system_resource != nullptr);
}

MultiNameSpace::~MultiNameSpace()
{
    if (system_resource != nullptr) {
        delete &HELPER(this);
        system_resource = nullptr;
    }
}

bool MultiNameSpace::setLocalMode(bool flag)
{
    return HELPER(this).setLocalMode(flag);
}

bool MultiNameSpace::localOnly() const
{
    HELPER(this).activate();
    return HELPER(this)._localOnly;
}

bool MultiNameSpace::usesCentralServer() const
{
    HELPER(this).activate();
    return HELPER(this)._usesCentralServer;
}

bool MultiNameSpace::connectionHasNameOfEndpoints() const
{
    HELPER(this).activate();
    return HELPER(this)._connectionHasNameOfEndpoints;
}

void MultiNameSpace::queryBypass(NameStore* store)
{
    altStore = store;
}

NameStore* MultiNameSpace::getQueryBypass()
{
    return altStore;
}

bool MultiNameSpace::serverAllocatesPortNumbers() const
{
    HELPER(this).activate();
    return HELPER(this)._serverAllocatesPortNumbers;
}

bool MultiNameSpace::activate(bool force)
{
    return HELPER(this).activate(force);
}

Contact MultiNameSpace::getNameServerContact() const
{
    return ((MultiNameSpaceHelper*)system_resource)->getNameServerContact();
}

Contact MultiNameSpace::queryName(const std::string& name)
{
    return HELPER(this).queryName(name);
}

bool MultiNameSpace::connectPortToTopic(const Contact& src,
                                        const Contact& dest,
                                        const ContactStyle& style)
{
    NameSpace* ns = HELPER(this).getOne();
    if (ns == nullptr) {
        return false;
    }
    return ns->connectPortToTopic(src, dest, style);
}

bool MultiNameSpace::connectTopicToPort(const Contact& src,
                                        const Contact& dest,
                                        const ContactStyle& style)
{
    NameSpace* ns = HELPER(this).getOne();
    if (ns == nullptr) {
        return false;
    }
    return ns->connectTopicToPort(src, dest, style);
}

bool MultiNameSpace::disconnectPortFromTopic(const Contact& src,
                                             const Contact& dest,
                                             const ContactStyle& style)
{
    NameSpace* ns = HELPER(this).getOne();
    if (ns == nullptr) {
        return false;
    }
    return ns->disconnectPortFromTopic(src, dest, style);
}

bool MultiNameSpace::disconnectTopicFromPort(const Contact& src,
                                             const Contact& dest,
                                             const ContactStyle& style)
{
    NameSpace* ns = HELPER(this).getOne();
    if (ns == nullptr) {
        return false;
    }
    return ns->disconnectTopicFromPort(src, dest, style);
}

bool MultiNameSpace::connectPortToPortPersistently(const Contact& src,
                                                   const Contact& dest,
                                                   const ContactStyle& style)
{
    NameSpace* ns = HELPER(this).getOne();
    if (ns == nullptr) {
        return false;
    }
    return ns->connectPortToPortPersistently(src, dest, style);
}

bool MultiNameSpace::disconnectPortToPortPersistently(const Contact& src,
                                                      const Contact& dest,
                                                      const ContactStyle& style)
{
    NameSpace* ns = HELPER(this).getOne();
    if (ns == nullptr) {
        return false;
    }
    return ns->disconnectPortToPortPersistently(src, dest, style);
}

Contact MultiNameSpace::registerName(const std::string& name)
{
    SpaceList lst = HELPER(this).getAll();
    Contact result;
    // loop through namespaces
    for (size_t i = 0; i < lst.size(); i++) {
        Contact iresult;
        // Register name with namespace. If contact information is
        // fleshed out while registering, we carry that along for
        // registration with the next namespace.
        if (result.getPort() <= 0) {
            iresult = lst[i]->registerName(name);
        } else {
            iresult = lst[i]->registerContact(result);
        }
        if (i == 0 || result.getPort() <= 0) {
            result = iresult;
        }
    }
    return result;
}

Contact MultiNameSpace::registerContact(const Contact& contact)
{
    SpaceList lst = HELPER(this).getAll();
    Contact result;
    for (size_t i = 0; i < lst.size(); i++) {
        // we register in *all* namespaces (and query in *any*)
        Contact iresult = lst[i]->registerContact(contact);
        if (i == 0) {
            result = iresult;
        }
    }
    return result;
}

Contact MultiNameSpace::unregisterName(const std::string& name)
{
    SpaceList lst = HELPER(this).getAll();
    Contact result;
    for (size_t i = 0; i < lst.size(); i++) {
        // we unregister in *all* namespaces
        Contact iresult = lst[i]->unregisterName(name);
        if (i == 0) {
            result = iresult;
        }
    }
    return result;
}

Contact MultiNameSpace::unregisterContact(const Contact& contact)
{
    SpaceList lst = HELPER(this).getAll();
    Contact result;
    for (size_t i = 0; i < lst.size(); i++) {
        // we unregister in *all* namespaces
        Contact iresult = lst[i]->unregisterContact(contact);
        if (i == 0) {
            result = iresult;
        }
    }
    return result;
}

bool MultiNameSpace::setProperty(const std::string& name, const std::string& key, const Value& value)
{
    NameSpace* ns = HELPER(this).getOne();
    if (ns == nullptr) {
        return false;
    }
    return ns->setProperty(name, key, value);
}

Value* MultiNameSpace::getProperty(const std::string& name, const std::string& key)
{
    NameSpace* ns = HELPER(this).getOne();
    if (ns == nullptr) {
        return nullptr;
    }
    return ns->getProperty(name, key);
}

Contact MultiNameSpace::detectNameServer(bool useDetectedServer,
                                         bool& scanNeeded,
                                         bool& serverUsed)
{
    // This code looks like a placeholder that never got replaced.
    // It is using a heuristic that namespaces with "/ros" in the
    // name are ros namespaces.  There's no need for guesswork like
    // that anymore.  Also, code duplication.  Should spin this
    // off into a proper plugin mechanism for namespaces.
    std::string name = NetworkBase::getNameServerName();
    Contact fake;
    Contact r;
    if (name.find("/ros") != std::string::npos) {
        RosNameSpace ns(fake);
        r = ns.detectNameServer(useDetectedServer, scanNeeded, serverUsed);
        if (r.isValid() && useDetectedServer && scanNeeded) {
            HELPER(this).activate(true);
        }
    } else {
        YarpNameSpace ns(fake);
        r = ns.detectNameServer(useDetectedServer, scanNeeded, serverUsed);
        if (r.isValid() && useDetectedServer && scanNeeded) {
            HELPER(this).activate(true);
        }
    }
    return r;
}


bool MultiNameSpace::writeToNameServer(PortWriter& cmd,
                                       PortReader& reply,
                                       const ContactStyle& style)
{
    NameSpace* ns = HELPER(this).getOne();
    if (ns == nullptr) {
        return false;
    }
    return ns->writeToNameServer(cmd, reply, style);
}
