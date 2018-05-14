/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * Copyright (C) 2006, 2008 Arjan Gijsberts
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/NetType.h>

#include <yarp/os/impl/BottleImpl.h>
#include <yarp/os/impl/Logger.h>


using yarp::os::Bottle;
using std::string;
using yarp::os::ConnectionReader;
using yarp::os::ConnectionWriter;
using yarp::os::Property;
using yarp::os::Value;
using yarp::os::impl::BottleImpl;
using yarp::os::impl::Storable;

class NullBottle : public Bottle
{
public:
    NullBottle() : Bottle() { setReadOnly(true); }
    virtual bool isNull() const override { return true; }
    static NullBottle* bottleNull;
};

NullBottle* NullBottle::bottleNull = nullptr;

Bottle::Bottle()
        : Portable(), Searchable(), implementation(new BottleImpl(this))
{
    yAssert(implementation != nullptr);
    implementation->invalid = false;
    implementation->ro = false;
}

Bottle::Bottle(const std::string& text)
        : Portable(), Searchable(), implementation(new BottleImpl(this))
{
    yAssert(implementation != nullptr);
    implementation->invalid = false;
    implementation->ro = false;
    fromString(text);
}

Bottle::Bottle(const Bottle& bottle)
        : Portable(), Searchable(), implementation(new BottleImpl(this))
{
    yAssert(implementation != nullptr);
    implementation->invalid = false;
    implementation->ro = false;
    copy(bottle);
}

Bottle& Bottle::operator=(const Bottle& bottle)
{
    implementation->edit();
    copy(bottle);
    return *this;
}

Bottle::~Bottle()
{
    if (implementation) {
        delete implementation;
    }
}

void Bottle::clear()
{
    implementation->edit();
    implementation->invalid = false;
    implementation->clear();
}

void Bottle::addInt(int x)
{
    implementation->edit();
    implementation->addInt(x);
}

void Bottle::addInt64(const YARP_INT64& x)
{
    implementation->edit();
    implementation->addInt64(x);
}

void Bottle::addVocab(int x)
{
    implementation->edit();
    implementation->addVocab(x);
}

void Bottle::addDouble(double x)
{
    implementation->edit();
    implementation->addDouble(x);
}

void Bottle::addString(const char* str)
{
    implementation->edit();
    implementation->addString(str);
}

void Bottle::addString(const std::string& str)
{
    implementation->edit();
    implementation->addString(str);
}

Bottle& Bottle::addList()
{
    implementation->edit();
    return implementation->addList();
}

Property& Bottle::addDict()
{
    implementation->edit();
    return implementation->addDict();
}

Value Bottle::pop()
{
    implementation->edit();
    Storable* stb = implementation->pop();
    Value val(*stb);
    // here we take responsibility for deallocation of the Storable instance
    delete stb;
    return val;
}

void Bottle::fromString(const std::string& text)
{
    implementation->edit();
    implementation->invalid = false;
    implementation->fromString(text.c_str());
}

std::string Bottle::toString() const
{
    return std::string(implementation->toString().c_str());
}

void Bottle::fromBinary(const char* buf, int len)
{
    implementation->edit();
    implementation->fromBinary(buf, len);
}

const char* Bottle::toBinary(size_t* size)
{
    if (size != nullptr) {
        *size = implementation->byteCount();
    }
    return implementation->getBytes();
}

bool Bottle::write(ConnectionWriter& writer)
{
    return implementation->write(writer);
}

void Bottle::onCommencement()
{
    implementation->onCommencement();
}

bool Bottle::read(ConnectionReader& reader)
{
    implementation->edit();
    return implementation->read(reader);
}

Value& Bottle::get(int index) const
{
    return implementation->get(index);
}

int Bottle::size() const
{
    return static_cast<int>(implementation->size());
}

void Bottle::hasChanged()
{
    implementation->hasChanged();
}

int Bottle::getSpecialization()
{
    return implementation->getSpecialization();
}

void Bottle::copy(const Bottle& alt, int first, int len)
{
    implementation->edit();
    if (alt.isNull()) {
        clear();
        implementation->invalid = true;
        return;
    }
    implementation->copyRange(alt.implementation, first, len);
}

bool Bottle::check(const std::string& key) const
{
    Bottle& val = findGroup(key);
    if (!val.isNull()) {
        return true;
    }
    Value& val2 = find(key);
    return !val2.isNull();
}

Value& Bottle::find(const std::string& key) const
{
    Value& val = implementation->findBit(key);

    if (getMonitor() != nullptr) {
        SearchReport report;
        report.key = key;
        report.isFound = !val.isNull();
        report.value = val.toString();
        reportToMonitor(report);
    }

    return val;
}

Bottle& Bottle::findGroup(const std::string& key) const
{
    Value& bb = implementation->findGroupBit(key);

    if (getMonitor() != nullptr) {
        SearchReport report;
        report.key = key;
        report.isGroup = true;
        if (bb.isList()) {
            report.isFound = true;
            report.value = bb.toString();
        }
        reportToMonitor(report);
        if (bb.isList()) {
            std::string context = getMonitorContext().c_str();
            context += ".";
            context += key;
            bb.asList()->setMonitor(getMonitor(),
                                    context.c_str()); // pass on any monitoring
        }
    }

    if (bb.isList()) {
        return *(bb.asList());
    }
    return getNullBottle();
}

void Bottle::add(Value* value)
{
    implementation->edit();
    implementation->addBit(value);
}

void Bottle::add(const Value& value)
{
    implementation->edit();
    implementation->addBit(value);
}

Bottle& Bottle::getNullBottle()
{
    if (NullBottle::bottleNull == nullptr) {
        NullBottle::bottleNull = new NullBottle();
    }
    return *NullBottle::bottleNull;
}

void Bottle::fini()
{
    if (NullBottle::bottleNull != nullptr) {
        delete NullBottle::bottleNull;
        NullBottle::bottleNull = nullptr;
    }
}

bool Bottle::operator==(const Bottle& alt) const
{
    return std::string(toString().c_str()) == alt.toString().c_str();
}

bool Bottle::write(PortReader& reader, bool textMode)
{
    DummyConnector con;
    con.setTextMode(textMode);
    write(con.getWriter());
    return reader.read(con.getReader());
}

bool Bottle::read(PortWriter& writer, bool textMode)
{
    implementation->edit();
    DummyConnector con;
    con.setTextMode(textMode);
    writer.write(con.getWriter());
    return read(con.getReader());
}

bool Bottle::isNull() const
{
    return implementation->invalid;
}

bool Bottle::operator!=(const Bottle& alt) const
{
    return !((*this) == alt);
}

void Bottle::append(const Bottle& alt)
{
    implementation->edit();
    for (int i = 0; i < alt.size(); i++) {
        add(alt.get(i));
    }
}

Bottle Bottle::tail() const
{
    Bottle b;
    if (isNull()) {
        return *this;
    }
    b.copy(*this, 1, size() - 1);
    return b;
}

std::string Bottle::toString(int x)
{
    return NetType::toString(x);
}

std::string Bottle::describeBottleCode(int code)
{
    int unit = code & ~(BOTTLE_TAG_LIST | BOTTLE_TAG_DICT);
    std::string unitName = "mixed";
    switch (unit) {
    case 0:
        unitName = "mixed";
        break;
    case BOTTLE_TAG_INT:
        unitName = "int";
        break;
    case BOTTLE_TAG_VOCAB:
        unitName = "vocab";
        break;
    case BOTTLE_TAG_DOUBLE:
        unitName = "float";
        break;
    case BOTTLE_TAG_STRING:
        unitName = "string";
        break;
    case BOTTLE_TAG_BLOB:
        unitName = "blob";
        break;
    default:
        unitName = "unknown";
        break;
    }
    std::string result = unitName;
    if (code & BOTTLE_TAG_LIST) {
        result = "list of " + unitName;
    } else if (code & BOTTLE_TAG_DICT) {
        result = "dict of " + unitName;
    }
    return result;
}

void Bottle::setReadOnly(bool readOnly)
{
    implementation->ro = readOnly;
}
