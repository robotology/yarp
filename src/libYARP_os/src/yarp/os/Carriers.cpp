/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Carriers.h>

#include <yarp/os/YarpPlugin.h>
#include <yarp/os/impl/FakeFace.h>
#include <yarp/os/impl/HttpCarrier.h>
#include <yarp/os/impl/LocalCarrier.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/McastCarrier.h>
#include <yarp/os/impl/NameserCarrier.h>
#include <yarp/os/impl/Protocol.h>
#include <yarp/os/impl/TcpCarrier.h>
#include <yarp/os/impl/TcpFace.h>
#include <yarp/os/impl/TextCarrier.h>
#include <yarp/os/impl/UdpCarrier.h>

#include <vector>
#include <mutex>

using namespace yarp::os::impl;
using namespace yarp::os;

namespace {
YARP_OS_LOG_COMPONENT(CARRIERS, "yarp.os.Carriers")
} // namespace

namespace {
std::string bytes_to_string(const Bytes& header)
{
    std::string ret;
    for (size_t i = 0; i < header.length(); i++) {
        ret += yarp::conf::numeric::to_string(header.get()[i]);
        ret += " ";
    }
    ret += "[";
    for (size_t i = 0; i < header.length(); i++) {
        char ch = header.get()[i];
        if (ch >= 32) {
            ret += ch;
        } else {
            ret += '.';
        }
    }
    ret += "]";
    return ret;
}
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class Carriers::Private : public YarpPluginSelector
{
public:
    static std::mutex mutex;

    std::vector<Carrier*> delegates;

    Carrier* chooseCarrier(const std::string& name,
                           bool load_if_needed = true,
                           bool return_template = false);
    Carrier* chooseCarrier(const Bytes& header,
                           bool load_if_needed = true);

    static bool matchCarrier(const Bytes& header, Bottle& code);
    static bool checkForCarrier(const Bytes& header, Searchable& group);
    static bool scanForCarrier(const Bytes& header);

    bool select(Searchable& options) override;
};

std::mutex Carriers::Private::mutex{};

Carrier* Carriers::Private::chooseCarrier(const std::string& name,
                                          bool load_if_needed,
                                          bool return_template)
{
    auto pos = name.find('+');
    if (pos != std::string::npos) {
        return chooseCarrier(name.substr(0, pos), load_if_needed, return_template);
    }

    for (auto& delegate : delegates) {
        Carrier& c = *delegate;
        if (name == c.getName()) {
            if (!return_template) {
                return c.create();
            }
            return &c;
        }
    }

    if (load_if_needed) {
        // ok, we didn't find a carrier, but we have a name.
        // let's try to register it, and see if a dll is found.
        if (NetworkBase::registerCarrier(name.c_str(), nullptr)) {
            // We made progress, let's try again...
            return Carriers::Private::chooseCarrier(name, false);
        }
    }

    yCError(CARRIERS,
            "Could not find carrier \"%s\"",
            (!name.empty()) ? name.c_str() : "[bytes]");

    return nullptr;
}

Carrier* Carriers::Private::chooseCarrier(const Bytes& header,
                                          bool load_if_needed)
{
    for (auto& delegate : delegates) {
        Carrier& c = *delegate;
        if (c.checkHeader(header)) {
            return c.create();
        }
    }

    if (load_if_needed) {
        if (scanForCarrier(header)) {
            // We made progress, let's try again...
            return Carriers::Private::chooseCarrier(header, false);
        }
    }

    yCError(CARRIERS,
            "Could not find carrier for a connection starting with: %s",
            bytes_to_string(header).c_str());

    return nullptr;
}


bool Carriers::Private::matchCarrier(const Bytes& header, Bottle& code)
{
    size_t at = 0;
    bool success = true;
    bool done = false;
    for (size_t i = 0; i < code.size() && !done; i++) {
        Value& v = code.get(i);
        if (v.isString()) {
            std::string str = v.asString();
            for (char j : str) {
                if (header.length() <= at) {
                    success = false;
                    done = true;
                    break;
                }
                if (j != header.get()[at]) {
                    success = false;
                    done = true;
                    break;
                }
                at++;
            }
        } else {
            at++;
        }
    }
    return success;
}

bool Carriers::Private::checkForCarrier(const Bytes& header, Searchable& group)
{
    Bottle code = group.findGroup("code").tail();
    if (code.size() == 0) {
        return false;
    }
    if (matchCarrier(header, code)) {
        std::string name = group.find("name").asString();
        if (NetworkBase::registerCarrier(name.c_str(), nullptr)) {
            return true;
        }
    }
    return false;
}

bool Carriers::Private::scanForCarrier(const Bytes& header)
{
    yCDebug(CARRIERS, "Scanning for a carrier by header.");
    YarpPluginSelector selector;
    selector.scan();
    Bottle lst = selector.getSelectedPlugins();
    for (size_t i = 0; i < lst.size(); i++) {
        if (checkForCarrier(header, lst.get(i))) {
            return true;
        }
    }
    return false;
}

bool Carriers::Private::select(Searchable& options)
{
    return options.check("type", Value("none")).asString() == "carrier";
}

#endif // DOXYGEN_SHOULD_SKIP_THIS


Carriers::Carriers() :
        mPriv(new Private)
{
    mPriv->delegates.emplace_back(new HttpCarrier());
    mPriv->delegates.emplace_back(new NameserCarrier());
    mPriv->delegates.emplace_back(new LocalCarrier());
    mPriv->delegates.emplace_back(new TcpCarrier());
    mPriv->delegates.emplace_back(new TcpCarrier(false));
    mPriv->delegates.emplace_back(new McastCarrier());
    mPriv->delegates.emplace_back(new UdpCarrier());
    mPriv->delegates.emplace_back(new TextCarrier());
    mPriv->delegates.emplace_back(new TextCarrier(true));
}

Carriers::~Carriers()
{
    clear();
    delete mPriv;
}

void Carriers::clear()
{
    for (auto& delegate : mPriv->delegates) {
        delete delegate;
        delegate = nullptr;
    }
    mPriv->delegates.clear();
}

Carrier* Carriers::chooseCarrier(const std::string& name)
{
    return getInstance().mPriv->chooseCarrier(name);
}

Carrier* Carriers::getCarrierTemplate(const std::string& name)
{
    return getInstance().mPriv->chooseCarrier(name, true, true);
}


Carrier* Carriers::chooseCarrier(const Bytes& bytes)
{
    return getInstance().mPriv->chooseCarrier(bytes);
}


Face* Carriers::listen(const Contact& address)
{
    Face* face = nullptr;
    Carrier* c = nullptr;

    if (address.getCarrier() == "fake") //for backward compatibility
    {
        face = new FakeFace();
    }

    else {
        if (!address.getCarrier().empty()) {
            c = getCarrierTemplate(address.getCarrier());
        }

        if (c != nullptr) {
            face = c->createFace();
        } else {
            //if address hasn't carrier then use the default one (tcpFace)
            face = new TcpFace();
        }
    }

    bool ok = face->open(address);
    if (!ok) {
        delete face;
        face = nullptr;
    }
    return face;
}


OutputProtocol* Carriers::connect(const Contact& address)
{
    yarp::os::Face* face = nullptr;
    Carrier* c = nullptr;

    if (!address.getCarrier().empty()) {
        c = getCarrierTemplate(address.getCarrier());
    }
    if (c != nullptr) {
        face = c->createFace();
    } else {
        //if address hasn't carrier than use the default one (tcpFace)
        face = new TcpFace();
    }

    OutputProtocol* proto = face->write(address);
    delete face;
    return proto;
}


bool Carriers::addCarrierPrototype(Carrier* carrier)
{
    getInstance().mPriv->delegates.emplace_back(carrier);
    return true;
}


Carriers& Carriers::getInstance()
{
    static Carriers instance;
    return instance;
}


Bottle Carriers::listCarriers()
{
    Carriers& instance = getInstance();
    std::lock_guard<std::mutex> guard(Private::mutex);

    Bottle lst;
    Property done;

    std::vector<Carrier*>& delegates = instance.mPriv->delegates;
    for (auto& delegate : delegates) {
        Carrier& c = *delegate;
        lst.addString(c.getName());
        done.put(c.getName(), 1);
    }

    instance.mPriv->scan();
    Bottle plugins = instance.mPriv->getSelectedPlugins();
    for (size_t i = 0; i < plugins.size(); i++) {
        Value& options = plugins.get(i);
        std::string name = options.check("name", Value("untitled")).asString();
        if (done.check(name)) {
            continue;
        }

        SharedLibraryFactory lib;
        YarpPluginSettings settings;
        settings.setSelector(*instance.mPriv);
        settings.readFromSearchable(options, name);
        settings.open(lib);
        if (lib.getName().empty()) {
            continue;
        }
        lst.addString(name);
        done.put(name, 1);
    }

    return lst;
}
