// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/MultiNameSpace.h>
#include <yarp/os/YarpNameSpace.h>
#include <yarp/os/RosNameSpace.h>
#include <yarp/os/impl/PlatformVector.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os;
using namespace yarp::os::impl;

typedef PlatformVector<NameSpace *> SpaceList;

class MultiNameSpaceHelper {
public:
    SpaceList spaces;
    bool _localOnly;
    bool _usesCentralServer;
    bool _serverAllocatesPortNumbers;
    bool _connectionHasNameOfEndpoints;

    MultiNameSpaceHelper() {
        clear();
    }

    ~MultiNameSpaceHelper() {
        clear();
    }

    void clear() {
        for (int i=0; i<(int)spaces.size(); i++) {
            NameSpace *ns = spaces[i];
            if (ns) {
                delete ns;
                ns = NULL;
            }
        }
        spaces.clear();
        _localOnly = true;
        _usesCentralServer = false;
        _serverAllocatesPortNumbers = false;
        _connectionHasNameOfEndpoints = true;
    }

    void scan() {
        _localOnly = true;
        _usesCentralServer = false;
        _serverAllocatesPortNumbers = true;
        for (int i=0; i<(int)spaces.size(); i++) {
            NameSpace *ns = spaces[i];
            if (!ns) continue;
            if (!ns->localOnly()) _localOnly = false;
            if (ns->usesCentralServer()) _usesCentralServer = true;
            if (!ns->serverAllocatesPortNumbers()) {
                _serverAllocatesPortNumbers = false;
            }
            if (!ns->connectionHasNameOfEndpoints()) {
                _connectionHasNameOfEndpoints = false;
            }
        }
    }

    bool setLocalMode(bool flag) {
        clear();
        if (flag) {
            NameSpace *ns = new YarpDummyNameSpace;
            spaces.push_back(ns);
        }
        scan();
        return true;
    }

    bool activate(bool force = false) {
        if (force) {
            clear();
        }
        if (spaces.size()!=0) return true;
        NameConfig conf;
        if (!conf.fromFile()) {
            return false;
        }
        Bottle ns = conf.getNamespaces();
        for (int i=0; i<ns.size(); i++) {
            ConstString n = ns.get(i).asString();
            //printf("NAMESPACE %s\n", n.c_str());
            NameConfig conf2;
            if (!conf2.fromFile(n.c_str())) {
                fprintf(stderr, "Could not find namespace %s\n",
                        n.c_str());
                continue;
            }
            String mode = conf2.getMode();
            Contact address = conf2.getAddress().toContact().addName(n);
            //printf("ADDRESS %s\n", address.toString().c_str());
            if (mode=="yarp"||mode=="//") {
                NameSpace *ns = new YarpNameSpace(address);
                spaces.push_back(ns);
            } else if (mode=="ros") {
                NameSpace *ns = new RosNameSpace(address);
                spaces.push_back(ns);
            } else {
                YARP_SPRINTF1(Logger::get(),error,
                              "cannot deal with namespace of type %s",
                              mode.c_str());
                return false;
            }
        }
        scan();
        return true;
    }

    Contact getNameServerContact() {
        activate();
        if (spaces.size()>0) {
            return spaces[0]->getNameServerContact();
        }
        return Contact();
    }

    Contact queryName(const ConstString& name) {
        activate();
        for (int i=0; i<(int)spaces.size(); i++) {
            NameSpace *ns = spaces[i];
            if (!ns) continue;
            if (ns->getNameServerName()==name) {
                return ns->getNameServerContact();
            }
            Contact result = ns->queryName(name);
            if (result.isValid()) return result;
        }
        return Contact();
    }

    NameSpace *getOne() {
        activate();
        if (spaces.size()==0) {
            return NULL;
        }
        return spaces[0];
    }

    SpaceList& getAll() {
        activate();
        return spaces;
    }
};

#define HELPER(x) (*((MultiNameSpaceHelper*)((x)->system_resource)))

MultiNameSpace::MultiNameSpace() {
    altStore = NULL;
    system_resource = new MultiNameSpaceHelper;
    YARP_ASSERT(system_resource!=NULL);
}

MultiNameSpace::~MultiNameSpace() {
    if (system_resource!=NULL) {
        delete &HELPER(this);
        system_resource = NULL;
    }
}

bool MultiNameSpace::setLocalMode(bool flag) {
    return HELPER(this).setLocalMode(flag);
}

bool MultiNameSpace::localOnly() const {
    HELPER(this).activate();
    return HELPER(this)._localOnly;
}

bool MultiNameSpace::usesCentralServer() const {
    HELPER(this).activate();
    return HELPER(this)._usesCentralServer;
}

bool MultiNameSpace::connectionHasNameOfEndpoints() const {
    HELPER(this).activate();
    return HELPER(this)._connectionHasNameOfEndpoints;
}

void MultiNameSpace::queryBypass(NameStore *store) {
    altStore = store;
}

NameStore *MultiNameSpace::getQueryBypass() {
    return altStore;
}

bool MultiNameSpace::serverAllocatesPortNumbers() const {
    HELPER(this).activate();
    return HELPER(this)._serverAllocatesPortNumbers;
}

bool MultiNameSpace::activate(bool force) {
    return HELPER(this).activate(force);
}

Contact MultiNameSpace::getNameServerContact() const {
    return ((MultiNameSpaceHelper*)system_resource)->getNameServerContact();
}

Contact MultiNameSpace::queryName(const ConstString& name) {
    return HELPER(this).queryName(name);
}

bool MultiNameSpace::connectPortToTopic(const Contact& src,
                                        const Contact& dest,
                                        ContactStyle style) {
    NameSpace *ns = HELPER(this).getOne();
    if (!ns) return false;
    return ns->connectPortToTopic(src,dest,style);
}

bool MultiNameSpace::connectTopicToPort(const Contact& src,
                                        const Contact& dest,
                                        ContactStyle style) {
    NameSpace *ns = HELPER(this).getOne();
    if (!ns) return false;
    return ns->connectTopicToPort(src,dest,style);
}

bool MultiNameSpace::disconnectPortFromTopic(const Contact& src,
                                             const Contact& dest,
                                             ContactStyle style) {
    NameSpace *ns = HELPER(this).getOne();
    if (!ns) return false;
    return ns->disconnectPortFromTopic(src,dest,style);
}

bool MultiNameSpace::disconnectTopicFromPort(const Contact& src,
                                             const Contact& dest,
                                             ContactStyle style) {
    NameSpace *ns = HELPER(this).getOne();
    if (!ns) return false;
    return ns->disconnectTopicFromPort(src,dest,style);
}

bool MultiNameSpace::connectPortToPortPersistently(const Contact& src,
                                                   const Contact& dest,
                                                   ContactStyle style) {
    NameSpace *ns = HELPER(this).getOne();
    if (!ns) return false;
    return ns->connectPortToPortPersistently(src,dest,style);
}

bool MultiNameSpace::disconnectPortToPortPersistently(const Contact& src,
                                                      const Contact& dest,
                                                      ContactStyle style) {
    NameSpace *ns = HELPER(this).getOne();
    if (!ns) return false;
    return ns->disconnectPortToPortPersistently(src,dest,style);
}

Contact MultiNameSpace::registerName(const ConstString& name) {
    SpaceList lst = HELPER(this).getAll();
    Contact result;
    for (int i=0; i<(int)lst.size(); i++) {
        Contact iresult;
        if (result.getPort()<=0) {
            iresult = lst[i]->registerName(name);
        } else {
            iresult = lst[i]->registerContact(result);
        }
        if (i==0 || result.getPort()<=0) {
            result = iresult;
        }
    }
    return result;
}

Contact MultiNameSpace::registerContact(const Contact& contact) {
    SpaceList lst = HELPER(this).getAll();
    Contact result;
    for (int i=0; i<(int)lst.size(); i++) {
        Contact iresult = lst[i]->registerContact(contact);
        if (i==0) result = iresult;
    }
    return result;
}

Contact MultiNameSpace::unregisterName(const ConstString& name) {
    SpaceList lst = HELPER(this).getAll();
    Contact result;
    for (int i=0; i<(int)lst.size(); i++) {
        Contact iresult = lst[i]->unregisterName(name);
        if (i==0) result = iresult;
    }
    return result;
}

Contact MultiNameSpace::unregisterContact(const Contact& contact) {
    SpaceList lst = HELPER(this).getAll();
    Contact result;
    for (int i=0; i<(int)lst.size(); i++) {
        Contact iresult = lst[i]->unregisterContact(contact);
        if (i==0) result = iresult;
    }
    return result;
}

bool MultiNameSpace::setProperty(const ConstString& name, const ConstString& key,
                                const Value& value) {
    NameSpace *ns = HELPER(this).getOne();
    if (!ns) return false;
    return ns->setProperty(name,key,value);
}

Value *MultiNameSpace::getProperty(const ConstString& name, const ConstString& key) {
    NameSpace *ns = HELPER(this).getOne();
    if (!ns) return NULL;
    return ns->getProperty(name,key);
}

Contact MultiNameSpace::detectNameServer(bool useDetectedServer,
                                        bool& scanNeeded,
                                        bool& serverUsed) {
    ConstString name = NetworkBase::getNameServerName();
    Contact fake, r;
    if (name.find("/ros")!=ConstString::npos) {
        RosNameSpace ns(fake);
        r = ns.detectNameServer(useDetectedServer,scanNeeded,serverUsed);
        if (r.isValid()&&useDetectedServer&&scanNeeded) {
            HELPER(this).activate(true);
        }
    } else {
        YarpNameSpace ns(fake);
        r = ns.detectNameServer(useDetectedServer,scanNeeded,serverUsed);
        if (r.isValid()&&useDetectedServer&&scanNeeded) {
            HELPER(this).activate(true);
        }
    }
    return r;
}


bool MultiNameSpace::writeToNameServer(PortWriter& cmd,
                                       PortReader& reply,
                                       const ContactStyle& style) {
    NameSpace *ns = HELPER(this).getOne();
    if (!ns) return false;
    return ns->writeToNameServer(cmd,reply,style);
}
