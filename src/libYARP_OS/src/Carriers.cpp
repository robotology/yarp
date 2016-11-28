/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Carriers.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/TcpFace.h>
#include <yarp/os/impl/FakeFace.h>
#include <yarp/os/impl/TcpCarrier.h>
#include <yarp/os/impl/TextCarrier.h>

#ifdef YARP_HAS_ACE
#  include <yarp/os/impl/McastCarrier.h>
#  include <yarp/os/impl/ShmemCarrier.h>
#endif

#include <yarp/os/impl/UdpCarrier.h>
#include <yarp/os/impl/LocalCarrier.h>
#include <yarp/os/impl/NameserCarrier.h>
#include <yarp/os/impl/HttpCarrier.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/Protocol.h>
#include <yarp/os/YarpPlugin.h>
#include <yarp/os/impl/PlatformVector.h>

using namespace yarp::os::impl;
using namespace yarp::os;



class Carriers::Private : public YarpPluginSelector
{
public:
    static Carriers* yarp_carriers_instance;

    PlatformVector<Carrier*> delegates;

    Carrier* chooseCarrier(const ConstString* name,
                           const Bytes* header,
                           bool load_if_needed = true,
                           bool return_template = false);

    static bool matchCarrier(const Bytes *header, Bottle& code);
    static bool checkForCarrier(const Bytes *header, Searchable& group);
    static bool scanForCarrier(const Bytes *header);

    virtual bool select(Searchable& options);
};

Carriers* Carriers::Private::yarp_carriers_instance = YARP_NULLPTR;


Carrier* Carriers::Private::chooseCarrier(const ConstString *name,
                                          const Bytes *header,
                                          bool load_if_needed,
                                          bool return_template)
{
    ConstString s;
    if (name != YARP_NULLPTR) {
        s = *name;
        size_t i = s.find("+");
        if (i!=ConstString::npos) {
            s[i] = '\0';
            s = s.c_str();
            name = &s;
        }
    }
    for (size_t i = 0; i < delegates.size(); i++) {
        Carrier& c = *delegates[i];
        bool match = false;
        if (name != YARP_NULLPTR) {
            if ((*name) == c.getName()) {
                match = true;
            }
        }
        if (header != YARP_NULLPTR) {
            if (c.checkHeader(*header)) {
                match = true;
            }
        }
        if (match) {
            if (!return_template) {
                return c.create();
            }
            return &c;
        }
    }
    if (load_if_needed) {
        if (name != YARP_NULLPTR) {
            // ok, we didn't find a carrier, but we have a name.
            // let's try to register it, and see if a dll is found.
            if (NetworkBase::registerCarrier(name->c_str(), YARP_NULLPTR)) {
                // We made progress, let's try again...
                return Carriers::Private::chooseCarrier(name,header,false);
            }
        } else {
            if (scanForCarrier(header)) {
                // We made progress, let's try again...
                return Carriers::Private::chooseCarrier(name,header,true);
            }
        }
    }
    if (name == YARP_NULLPTR) {
        ConstString txt;
        for (int i=0; i<(int)header->length(); i++) {
            txt += NetType::toString(header->get()[i]);
            txt += " ";
        }
        txt += "[";
        for (int i=0; i<(int)header->length(); i++) {
            char ch = header->get()[i];
            if (ch>=32) {
                txt += ch;
            } else {
                txt += '.';
            }
        }
        txt += "]";

        YARP_SPRINTF1(Logger::get(),
                      error,
                      "Could not find carrier for a connection starting with: %s",
                      txt.c_str());
    } else {
        YARP_SPRINTF1(Logger::get(),
                      error,
                      "Could not find carrier \"%s\"",
                      (name != YARP_NULLPTR) ? name->c_str() : "[bytes]");;
    }
    return YARP_NULLPTR;
}



bool Carriers::Private::matchCarrier(const Bytes *header, Bottle& code)
{
    int at = 0;
    bool success = true;
    bool done = false;
    for (int i=0; i<code.size() && !done; i++) {
        Value& v = code.get(i);
        if (v.isString()) {
            ConstString str = v.asString();
            for (int j=0; j<(int)str.length(); j++) {
                if ((int)header->length()<=at) {
                    success = false;
                    done = true;
                    break;
                }
                if (str[j] != header->get()[at]) {
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

bool Carriers::Private::checkForCarrier(const Bytes *header, Searchable& group)
{
    Bottle code = group.findGroup("code").tail();
    if (code.size()==0) return false;
    if (matchCarrier(header,code)) {
        ConstString name = group.find("name").asString();
        if (NetworkBase::registerCarrier(name.c_str(), YARP_NULLPTR)) {
            return true;
        }
    }
    return false;
}

bool Carriers::Private::scanForCarrier(const Bytes *header)
{
    YARP_SPRINTF0(Logger::get(),
                  debug,
                  "Scanning for a carrier by header.");
    YarpPluginSelector selector;
    selector.scan();
    Bottle lst = selector.getSelectedPlugins();
    for (int i=0; i<lst.size(); i++) {
        if (checkForCarrier(header,lst.get(i))) {
            return true;
        }
    }
    return false;
}

bool Carriers::Private::select(Searchable& options)
{
    return options.check("type",Value("none")).asString() == "carrier";
}


Carriers::Carriers() :
        mPriv(new Private)
{
    mPriv->delegates.push_back(new HttpCarrier());
    mPriv->delegates.push_back(new NameserCarrier());
    mPriv->delegates.push_back(new LocalCarrier());
#ifdef YARP_HAS_ACE
    //mPriv->delegates.push_back(new ShmemCarrier(1));
    mPriv->delegates.push_back(new ShmemCarrier(2)); // new Alessandro version
#endif
    mPriv->delegates.push_back(new TcpCarrier());
    mPriv->delegates.push_back(new TcpCarrier(false));
#ifdef YARP_HAS_ACE
    mPriv->delegates.push_back(new McastCarrier());
#endif
    mPriv->delegates.push_back(new UdpCarrier());
    mPriv->delegates.push_back(new TextCarrier());
    mPriv->delegates.push_back(new TextCarrier(true));
}

Carriers::~Carriers()
{
    clear();
    delete mPriv;
}

void Carriers::clear()
{
    PlatformVector<Carrier*>& lst = mPriv->delegates;
    for (unsigned int i=0; i<lst.size(); i++) {
        delete lst[i];
    }
    lst.clear();
}

Carrier *Carriers::chooseCarrier(const ConstString& name)
{
    return getInstance().mPriv->chooseCarrier(&name, YARP_NULLPTR);
}

Carrier *Carriers::getCarrierTemplate(const ConstString& name)
{
    return getInstance().mPriv->chooseCarrier(&name, YARP_NULLPTR, true, true);
}


Carrier *Carriers::chooseCarrier(const Bytes& bytes)
{
    return getInstance().mPriv->chooseCarrier(YARP_NULLPTR, &bytes);
}


Face *Carriers::listen(const Contact& address)
{
    // for now, only TcpFace exists - otherwise would need to manage
    // multiple possibilities
    Face *face = YARP_NULLPTR;
    if (address.getCarrier() == "fake") {
        face = new FakeFace();
    }
    if (face == YARP_NULLPTR) {
        face = new TcpFace();
    }
    bool ok = face->open(address);
    if (!ok) {
        delete face;
        face = YARP_NULLPTR;
    }
    return face;
}


OutputProtocol *Carriers::connect(const Contact& address)
{
    TcpFace tcpFace;
    return tcpFace.write(address);
}


bool Carriers::addCarrierPrototype(Carrier *carrier)
{
    getInstance().mPriv->delegates.push_back(carrier);
    return true;
}


bool Carrier::reply(ConnectionState& proto, SizedWriter& writer)
{
    writer.write(proto.os());
    return proto.os().isOk();
}

Carriers& Carriers::getInstance()
{
    if (Private::yarp_carriers_instance == YARP_NULLPTR) {
        Private::yarp_carriers_instance = new Carriers();
        yAssert(Private::yarp_carriers_instance != YARP_NULLPTR);
    }
    return *Private::yarp_carriers_instance;
}


void Carriers::removeInstance()
{
    if (Private::yarp_carriers_instance != YARP_NULLPTR) {
        delete Private::yarp_carriers_instance;
        Private::yarp_carriers_instance = YARP_NULLPTR;
    }
}


Bottle Carriers::listCarriers()
{
    Bottle lst;
    Property done;

    PlatformVector<Carrier*>& delegates = getInstance().mPriv->delegates;
    for (size_t i = 0; i < delegates.size(); i++) {
        Carrier& c = *delegates[i];
        lst.addString(c.getName());
        done.put(c.getName(), 1);
    }

    getInstance().mPriv->scan();
    Bottle plugins = getInstance().mPriv->getSelectedPlugins();
    for (int i = 0; i < plugins.size(); i++) {
        Value& options = plugins.get(i);
        ConstString name = options.check("name",Value("untitled")).asString();
        if (done.check(name)) {
            continue;
        }

        SharedLibraryFactory lib;
        YarpPluginSettings settings;
        settings.setSelector(*getInstance().mPriv);
        settings.readFromSearchable(options,name);
        settings.open(lib);
        ConstString location = lib.getName().c_str();
        if (location=="") {
            continue;
        }
        lst.addString(name);
        done.put(name, 1);
    }

    return lst;
}
