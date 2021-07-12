/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2006, 2008 Arjan Gijsberts
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/BottleImpl.h>
#include <yarp/os/impl/LogComponent.h>


using yarp::os::Bottle;
using yarp::os::ConnectionReader;
using yarp::os::ConnectionWriter;
using yarp::os::Property;
using yarp::os::Value;
using yarp::os::impl::BottleImpl;
using yarp::os::impl::Storable;

namespace {
YARP_OS_LOG_COMPONENT(BOTTLE, "yarp.os.Bottle")
}

// FIXME this can be constexpr, but swig 3.0.8 is not happy
const Bottle::size_type Bottle::npos = static_cast<Bottle::size_type>(-1);

class NullBottle : public Bottle
{
public:
    NullBottle() :
            Bottle()
    {
        setReadOnly(true);
    }
    bool isNull() const override
    {
        return true;
    }
};

Bottle::Bottle() :
        Portable(),
        Searchable(),
        implementation(new BottleImpl(this))
{
    yCAssert(BOTTLE, implementation != nullptr);
    implementation->invalid = false;
    implementation->ro = false;
}

Bottle::Bottle(const std::string& text) :
        Portable(),
        Searchable(),
        implementation(new BottleImpl(this))
{
    yCAssert(BOTTLE, implementation != nullptr);
    implementation->invalid = false;
    implementation->ro = false;
    fromString(text);
}

Bottle::Bottle(const Bottle& rhs) :
        Portable(),
        Searchable(rhs),
        implementation(new BottleImpl(this))
{
    yCAssert(BOTTLE, implementation != nullptr);
    implementation->invalid = false;
    implementation->ro = false;
    copy(rhs);
}

Bottle::Bottle(Bottle&& rhs) noexcept :
        Portable(std::move(static_cast<Portable&>(rhs))),
        Searchable(std::move(static_cast<Searchable&>(rhs))),
        implementation(rhs.implementation)
{
    implementation->parent = this;
    rhs.implementation = new BottleImpl(&rhs);
}

Bottle::Bottle(std::initializer_list<Value> values) :
        Portable(),
        Searchable(),
        implementation(new BottleImpl(this))
{
    yCAssert(BOTTLE, implementation != nullptr);
    implementation->invalid = false;
    implementation->ro = false;

    for (const auto& val : values) {
        add(val);
    }
}

Bottle& Bottle::operator=(const Bottle& rhs)
{
    if (&rhs != this) {
        implementation->edit();
        copy(rhs);
    }
    return *this;
}

Bottle& Bottle::operator=(Bottle&& rhs) noexcept
{
    std::swap(implementation, rhs.implementation);
    implementation->parent = this;
    rhs.implementation->parent = &rhs;
    return *this;
}

Bottle::~Bottle()
{
    delete implementation;
}

void Bottle::clear()
{
    implementation->edit();
    implementation->invalid = false;
    implementation->clear();
}

void Bottle::addInt8(std::int8_t x)
{
    implementation->edit();
    implementation->addInt8(x);
}

void Bottle::addInt16(std::int16_t x)
{
    implementation->edit();
    implementation->addInt16(x);
}

void Bottle::addInt32(std::int32_t x)
{
    implementation->edit();
    implementation->addInt32(x);
}

void Bottle::addInt64(std::int64_t x)
{
    implementation->edit();
    implementation->addInt64(x);
}

void Bottle::addFloat32(yarp::conf::float32_t x)
{
    implementation->edit();
    implementation->addFloat32(x);
}

void Bottle::addFloat64(yarp::conf::float64_t x)
{
    implementation->edit();
    implementation->addFloat64(x);
}

void Bottle::addVocab32(yarp::conf::vocab32_t x)
{
    implementation->edit();
    implementation->addVocab32(x);
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
    implementation->fromString(text);
}

std::string Bottle::toString() const
{
    return implementation->toString();
}

void Bottle::fromBinary(const char* buf, size_t len)
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

bool Bottle::write(ConnectionWriter& writer) const
{
    return implementation->write(writer);
}

void Bottle::onCommencement() const
{
    implementation->onCommencement();
}

bool Bottle::read(ConnectionReader& reader)
{
    implementation->edit();
    return implementation->read(reader);
}

Value& Bottle::get(size_t index) const
{
    return implementation->get(index);
}

size_t Bottle::size() const
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

void Bottle::copy(const Bottle& alt, size_t first, size_t len)
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
            std::string context = getMonitorContext();
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
    static NullBottle bottleNull;
    return bottleNull;
}

bool Bottle::operator==(const Bottle& alt) const
{
    return toString() == alt.toString();
}

bool Bottle::write(PortReader& reader, bool textMode)
{
    DummyConnector con;
    con.setTextMode(textMode);
    write(con.getWriter());
    return reader.read(con.getReader());
}

bool Bottle::read(const PortWriter& writer, bool textMode)
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
    for (size_t i = 0; i < alt.size(); i++) {
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
    return yarp::conf::numeric::to_string(x);
}

std::string Bottle::describeBottleCode(int code)
{
    int unit = code & ~(BOTTLE_TAG_LIST | BOTTLE_TAG_DICT);
    std::string unitName = "mixed";
    switch (unit) {
    case 0:
        unitName = "mixed";
        break;
    case BOTTLE_TAG_INT32:
        unitName = "int";
        break;
    case BOTTLE_TAG_VOCAB32:
        unitName = "vocab";
        break;
    case BOTTLE_TAG_FLOAT64:
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
    if ((code & BOTTLE_TAG_LIST) != 0) {
        result = "list of " + unitName;
    } else if ((code & BOTTLE_TAG_DICT) != 0) {
        result = "dict of " + unitName;
    }
    return result;
}

void Bottle::setReadOnly(bool readOnly)
{
    implementation->ro = readOnly;
}
