/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Value.h>
#include <yarp/os/Bottle.h>

#include <yarp/os/impl/BottleImpl.h>

using namespace yarp::os;
using namespace yarp::os::impl;


Value::Value() :
        Portable(),
        Searchable(),
        proxy(nullptr) {
}

Value::Value(int x, bool isVocab) :
        Portable(),
        Searchable(),
        proxy(nullptr)
{
    if (!isVocab) {
        setProxy(static_cast<Storable*>(makeInt(x)));
    } else {
        setProxy(static_cast<Storable*>(makeVocab(x)));
    }
}

Value::Value(double x) :
        Portable(),
        Searchable(),
        proxy(nullptr)
{
    setProxy(static_cast<Storable*>(makeDouble(x)));
}

Value::Value(const std::string& str, bool isVocab) :
        Portable(),
        Searchable(),
        proxy(nullptr)
{
    if (!isVocab) {
        setProxy(static_cast<Storable*>(makeString(str)));
    } else {
        setProxy(static_cast<Storable*>(makeVocab(str)));
    }
}

Value::Value(void *data, int length) :
        Portable(),
        Searchable(),
        proxy(nullptr)
{
    setProxy(static_cast<Storable*>(makeBlob(data, length)));
}

Value::Value(const Value& alt) :
        Portable(),
        Searchable(),
        proxy(nullptr)
{
    setProxy(static_cast<Storable*>(alt.clone()));
}


const Value& Value::operator=(const Value& alt)
{
    if (&alt != this) {
        if (proxy == nullptr) {
            if (isLeaf() && alt.proxy) {
                // we are guaranteed to be a Storable
                ((Storable*)this)->copy(*((Storable*)alt.proxy));
            } else {
                setProxy(static_cast<Storable*>(alt.clone()));
            }
        } else {
            if (alt.proxy) {
                if (getCode() == alt.getCode()) {
                    // proxies are guaranteed to be Storable
                    ((Storable*)proxy)->copy(*((Storable*)alt.proxy));
                } else {
                    setProxy(static_cast<Storable*>(alt.clone()));
                }
            } else {
                if (proxy) {
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

bool Value::isInt() const
{
    ok();
    return proxy->isInt();
}

bool Value::isInt64() const
{
    ok();
    return proxy->isInt64();
}

bool Value::isString() const
{
    ok();
    return proxy->isString();
}

bool Value::isDouble() const
{
    ok();
    return proxy->isDouble();
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

bool Value::isVocab() const
{
    ok();
    return proxy->isVocab();
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

int Value::asInt() const
{
    ok();
    return proxy->asInt();
}

std::int64_t Value::asInt64() const
{
    ok();
    return proxy->asInt64();
}

int Value::asVocab() const
{
    ok();
    return proxy->asVocab();
}

double Value::asDouble() const
{
    ok();
    return proxy->asDouble();
}

std::string Value::asString() const
{
    ok();
    return proxy->asString();
}

Bottle *Value::asList() const
{
    ok();
    return proxy->asList();
}

Property *Value::asDict() const
{
    ok();
    return proxy->asDict();
}

Searchable *Value::asSearchable() const
{
    ok();
    if (proxy->isDict()) return proxy->asDict();
    return proxy->asList();
}

const char *Value::asBlob() const
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
    if (proxy) {
        delete proxy;
        proxy = nullptr;
    }
    int x = connection.expectInt();
    if ((x&0xffff) != x) return false;
    if (!(x&BOTTLE_TAG_LIST)) return false;
    int len = connection.expectInt();
    if (len==0) return true;
    if (len!=1) return false;
    if (x==BOTTLE_TAG_LIST) {
        x = connection.expectInt();
    } else {
        x &= ~BOTTLE_TAG_LIST;
    }
    if (connection.isError()) return false;
    Storable *s = Storable::createByCode(x);
    setProxy(s);
    if (!proxy) return false;
    return s->readRaw(connection);
}

bool Value::write(ConnectionWriter& connection)
{
    if (!proxy) {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(0);
        return !connection.isError();
    }
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(1);
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
    return (*proxy)==alt;
}


bool Value::operator!=(const Value& alt) const
{
    return !((*this)==alt);
}

void Value::fromString(const char *str) {
    setProxy(static_cast<Storable*>(makeValue(str)));
}

std::string Value::toString() const
{
    ok();
    return proxy->toString();
}

Value *Value::create() const
{
    ok();
    return proxy->create();
}

Value *Value::clone() const
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

Value *Value::makeInt(int x)
{
    return new StoreInt(x);
}


Value *Value::makeDouble(double x)
{
    return new StoreDouble(x);
}


Value *Value::makeString(const std::string& str)
{
    return new StoreString(str);
}


Value *Value::makeVocab(int v)
{
    return new StoreVocab(v);
}


Value *Value::makeVocab(const std::string& str)
{
    return new StoreVocab(Vocab::encode(str));
}


Value *Value::makeBlob(void *data, int length)
{
    std::string s((char*)data, length);
    return new StoreBlob(s);
}


Value *Value::makeList()
{
    return new StoreList();
}


Value *Value::makeList(const char *txt)
{
    Value *v = makeList();
    if (v!=nullptr) {
        v->asList()->fromString(txt);
    }
    return v;
}


Value *Value::makeValue(const std::string& txt)
{
    Bottle bot(txt);
    if (bot.size()>1) {
        return makeString(txt);
    }
    return bot.get(0).clone();
}


Value& Value::getNullValue()
{
    return BottleImpl::getNull();
}


void Value::setProxy(Storable *proxy)
{
    if (this->proxy!=nullptr) {
        delete this->proxy;
        this->proxy = nullptr;
    }
    this->proxy = proxy;
}


void Value::ok() const
{
    const Value *op = this;
    if (proxy==nullptr) {
        ((Value*)op)->setProxy(static_cast<Storable*>(makeList()));
    }
}
