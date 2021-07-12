/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Type.h>

#include <yarp/os/Property.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Value.h>

using namespace yarp::os;

class Type::Private
{
public:
    Private() = default;

    Private(const Private& rhs) :
            prop(nullptr),
            name(rhs.name),
            name_on_wire(rhs.name_on_wire)
    {
        if (rhs.prop != nullptr) {
            writeProperties();
            *prop = *(rhs.prop);
        }
    }

    Private(Private&& rhs) noexcept :
            prop(rhs.prop),
            name(std::move(rhs.name)),
            name_on_wire(std::move(rhs.name_on_wire))
    {
        rhs.prop = nullptr;
    }

    ~Private()
    {
        delete prop;
    }

    Private& operator=(const Private& rhs)
    {
        if (&rhs != this) {
            name = rhs.name;
            name_on_wire = rhs.name_on_wire;
            if (rhs.prop != nullptr) {
                writeProperties();
                *prop = *(rhs.prop);
            } else if (prop != nullptr) {
                delete prop;
                prop = nullptr;
            }
        }
        return *this;
    }

    Property& writeProperties()
    {
        if (prop == nullptr) {
            prop = new Property();
        }
        return *prop;
    }

    const Searchable& readProperties() const
    {
        if (prop == nullptr) {
            return Bottle::getNullBottle();
        }
        return *prop;
    }

    void addProperty(const char* key, const Value& val)
    {
        writeProperties();
        prop->put(key, val);
    }

    Property* prop{nullptr};
    std::string name;
    std::string name_on_wire;
};


Type::Type() :
        mPriv(new Private())
{
}

Type::Type(const Type& rhs) :
        mPriv(new Private(*(rhs.mPriv)))
{
}

Type::Type(Type&& rhs) noexcept :
        mPriv(rhs.mPriv)
{
    rhs.mPriv = nullptr;
}

Type::~Type()
{
    delete mPriv;
}

Type& Type::operator=(const Type& rhs)
{
    if (&rhs != this) {
        *mPriv = *(rhs.mPriv);
    }
    return *this;
}

Type& Type::operator=(Type&& rhs) noexcept
{
    if (&rhs != this) {
        std::swap(mPriv, rhs.mPriv);
    }
    return *this;
}

const Searchable& Type::readProperties() const
{
    return mPriv->readProperties();
}

Property& Type::writeProperties()
{
    return mPriv->writeProperties();
}

Type& Type::addProperty(const char* key, const Value& val)
{
    mPriv->addProperty(key, val);
    return *this;
}
std::string Type::getName() const
{
    return mPriv->name;
}

std::string Type::getNameOnWire() const
{
    return mPriv->name_on_wire;
}

bool Type::hasName() const
{
    return !mPriv->name.empty();
}

bool Type::isValid() const
{
    return hasName();
}

std::string Type::toString() const
{
    if (!mPriv->name_on_wire.empty()) {
        return mPriv->name + ":" + mPriv->name_on_wire;
    }
    if (!mPriv->name.empty()) {
        return mPriv->name;
    }
    return "null";
}


Type Type::byName(const char* name)
{
    Type t;
    t.mPriv->name = name;
    return t;
}

Type Type::byName(const char* name, const char* name_on_wire)
{
    Type t;
    t.mPriv->name = name;
    t.mPriv->name_on_wire = name_on_wire;
    return t;
}

Type Type::byNameOnWire(const char* name_on_wire)
{
    Type t;
    t.mPriv->name = "yarp/bottle";
    t.mPriv->name_on_wire = name_on_wire;
    return t;
}

Type Type::anon()
{
    return Type();
}
