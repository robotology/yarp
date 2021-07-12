/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Value.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/impl/BottleImpl.h>

using namespace yarp::os;
using namespace yarp::os::impl;


Value::Value() :
        Portable(),
        Searchable(),
        proxy(nullptr)
{
}

Value::Value(std::int32_t x, bool isVocab32) :
        Portable(),
        Searchable(),
        proxy(nullptr)
{
    if (!isVocab32) {
        setProxy(static_cast<Storable*>(makeInt32(x)));
    } else {
        setProxy(static_cast<Storable*>(makeVocab32(x)));
    }
}

Value::Value(yarp::conf::float64_t x) :
        Portable(),
        Searchable(),
        proxy(nullptr)
{
    setProxy(static_cast<Storable*>(makeFloat64(x)));
}

Value::Value(const std::string& str, bool isVocab32) :
        Portable(),
        Searchable(),
        proxy(nullptr)
{
    if (!isVocab32) {
        setProxy(static_cast<Storable*>(makeString(str)));
    } else {
        setProxy(static_cast<Storable*>(makeVocab32(str)));
    }
}

Value::Value(void* data, int length) :
        Portable(),
        Searchable(),
        proxy(nullptr)
{
    setProxy(static_cast<Storable*>(makeBlob(data, length)));
}

Value::Value(const Value& alt) :
        Portable(),
        Searchable(alt),
        proxy(nullptr)
{
    setProxy(static_cast<Storable*>(alt.clone()));
}


const Value& Value::operator=(const Value& alt)
{
    if (&alt != this) {
        if (proxy == nullptr) {
            if (isLeaf() && (alt.proxy != nullptr)) {
                // we are guaranteed to be a Storable
                ((Storable*)this)->copy(*((Storable*)alt.proxy));
            } else {
                setProxy(static_cast<Storable*>(alt.clone()));
            }
        } else {
            if (alt.proxy != nullptr) {
                if (getCode() == alt.getCode()) {
                    // proxies are guaranteed to be Storable
                    ((Storable*)proxy)->copy(*((Storable*)alt.proxy));
                } else {
                    setProxy(static_cast<Storable*>(alt.clone()));
                }
            } else {
                if (proxy != nullptr) {
                    delete proxy;
                    proxy = nullptr;
                }
                if (alt.isLeaf()) {
                    setProxy(static_cast<Storable*>(alt.clone()));
                }
            }
        }
    }
    return *this;
}

Value::~Value()
{
    if (proxy != nullptr) {
        delete proxy;
        proxy = nullptr;
    }
}

bool Value::isBool() const
{
    ok();
    return proxy->isBool();
}

bool Value::isInt8() const
{
    ok();
    return proxy->isInt8();
}

bool Value::isInt16() const
{
    ok();
    return proxy->isInt16();
}

bool Value::isInt32() const
{
    ok();
    return proxy->isInt32();
}

bool Value::isInt64() const
{
    ok();
    return proxy->isInt64();
}

bool Value::isFloat32() const
{
    ok();
    return proxy->isFloat32();
}

bool Value::isFloat64() const
{
    ok();
    return proxy->isFloat64();
}

bool Value::isString() const
{
    ok();
    return proxy->isString();
}

bool Value::isList() const
{
    ok();
    return proxy->isList();
}

bool Value::isDict() const
{
    ok();
    return proxy->isDict();
}

bool Value::isVocab32() const
{
    ok();
    return proxy->isVocab32();
}

bool Value::isBlob() const
{
    ok();
    return proxy->isBlob();
}

bool Value::asBool() const
{
    ok();
    return proxy->asBool();
}

std::int8_t Value::asInt8() const
{
    ok();
    return proxy->asInt8();
}

std::int16_t Value::asInt16() const
{
    ok();
    return proxy->asInt16();
}

std::int32_t Value::asInt32() const
{
    ok();
    return proxy->asInt32();
}

std::int64_t Value::asInt64() const
{
    ok();
    return proxy->asInt64();
}

yarp::conf::float32_t Value::asFloat32() const
{
    ok();
    return proxy->asFloat32();
}

yarp::conf::float64_t Value::asFloat64() const
{
    ok();
    return proxy->asFloat64();
}

yarp::conf::vocab32_t Value::asVocab32() const
{
    ok();
    return proxy->asVocab32();
}

std::string Value::asString() const
{
    ok();
    return proxy->asString();
}

Bottle* Value::asList() const
{
    ok();
    return proxy->asList();
}

Property* Value::asDict() const
{
    ok();
    return proxy->asDict();
}

Searchable* Value::asSearchable() const
{
    ok();
    if (proxy->isDict()) {
        return proxy->asDict();
    }
    return proxy->asList();
}

const char* Value::asBlob() const
{
    ok();
    return proxy->asBlob();
}

size_t Value::asBlobLength() const
{
    ok();
    return proxy->asBlobLength();
}

bool Value::read(ConnectionReader& connection)
{
    if (proxy != nullptr) {
        delete proxy;
        proxy = nullptr;
    }
    std::int32_t x = connection.expectInt32();
    if ((x & 0xffff) != x) {
        return false;
    }
    if ((x & BOTTLE_TAG_LIST) == 0) {
        return false;
    }
    std::int32_t len = connection.expectInt32();
    if (len == 0) {
        return true;
    }
    if (len != 1) {
        return false;
    }
    if (x == BOTTLE_TAG_LIST) {
        x = connection.expectInt32();
    } else {
        x &= ~BOTTLE_TAG_LIST;
    }
    if (connection.isError()) {
        return false;
    }
    Storable* s = Storable::createByCode(x);
    setProxy(s);
    if (proxy == nullptr) {
        return false;
    }
    return s->readRaw(connection);
}

bool Value::write(ConnectionWriter& connection) const
{
    if (proxy == nullptr) {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(0);
        return !connection.isError();
    }
    connection.appendInt32(BOTTLE_TAG_LIST);
    connection.appendInt32(1);
    return proxy->write(connection);
}

bool Value::check(const std::string& key) const
{
    ok();
    return proxy->check(key);
}

Value& Value::find(const std::string& key) const
{
    ok();
    return proxy->find(key);
}

Bottle& Value::findGroup(const std::string& key) const
{
    ok();
    return proxy->findGroup(key);
}

bool Value::operator==(const Value& alt) const
{
    ok();
    return (*proxy) == alt;
}


bool Value::operator!=(const Value& alt) const
{
    return !((*this) == alt);
}

void Value::fromString(const char* str)
{
    setProxy(static_cast<Storable*>(makeValue(str)));
}

std::string Value::toString() const
{
    ok();
    return proxy->toString();
}

Value* Value::create() const
{
    ok();
    return proxy->create();
}

Value* Value::clone() const
{
    ok();
    return proxy->clone();
}

int Value::getCode() const
{
    ok();
    return proxy->getCode();
}

bool Value::isNull() const
{
    ok();
    return proxy->isNull();
}

bool Value::isLeaf() const
{
    return false;
}

Value* Value::makeInt8(std::int8_t x)
{
    return new StoreInt8(x);
}

Value* Value::makeInt16(std::int16_t x)
{
    return new StoreInt16(x);
}

Value* Value::makeInt32(std::int32_t x)
{
    return new StoreInt32(x);
}

Value* Value::makeInt64(std::int64_t x)
{
    return new StoreInt64(x);
}

Value* Value::makeFloat32(yarp::conf::float32_t x)
{
    return new StoreFloat32(x);
}

Value* Value::makeFloat64(yarp::conf::float64_t x)
{
    return new StoreFloat64(x);
}

Value* Value::makeString(const std::string& str)
{
    return new StoreString(str);
}


Value* Value::makeVocab32(yarp::conf::vocab32_t v)
{
    return new StoreVocab32(v);
}


Value* Value::makeBlob(void* data, int length)
{
    std::string s((char*)data, length);
    return new StoreBlob(s);
}


Value* Value::makeList()
{
    return new StoreList();
}


Value* Value::makeList(const char* txt)
{
    Value* v = makeList();
    if (v != nullptr) {
        v->asList()->fromString(txt);
    }
    return v;
}


Value* Value::makeValue(const std::string& txt)
{
    Bottle bot(txt);
    if (bot.size() > 1) {
        return makeString(txt);
    }
    return bot.get(0).clone();
}


Value& Value::getNullValue()
{
    return BottleImpl::getNull();
}


void Value::setProxy(Storable* proxy)
{
    if (this->proxy != nullptr) {
        delete this->proxy;
        this->proxy = nullptr;
    }
    this->proxy = proxy;
}


void Value::ok() const
{
    const Value* op = this;
    if (proxy == nullptr) {
        ((Value*)op)->setProxy(static_cast<Storable*>(makeList()));
    }
}
