// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/BottleImpl.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Value.h>

using namespace yarp::os;
using namespace yarp::os::impl;


Value::Value() :
        Portable(),
        Searchable(),
        proxy(NULL) {
}

Value::Value(int x, bool isVocab) :
        Portable(),
        Searchable(),
        proxy(NULL) {
    if (!isVocab) {
        setProxy(makeInt(x));
    } else {
        setProxy(makeVocab(x));
    }
}

Value::Value(double x) :
        Portable(),
        Searchable(),
        proxy(NULL) {
    setProxy(makeDouble(x));
}

Value::Value(const ConstString& str, bool isVocab) :
        Portable(),
        Searchable(),
        proxy(NULL) {
    if (!isVocab) {
        setProxy(makeString(str));
    } else {
        setProxy(makeVocab(str));
    }
}

Value::Value(void *data, int length) :
        Portable(),
        Searchable(),
        proxy(NULL) {
    setProxy(makeBlob(data, length));
}

Value::Value(const Value& alt) :
        Portable(),
        Searchable(),
        proxy(NULL) {
    setProxy(alt.clone());
}


const Value& Value::operator = (const Value& alt) {
    if(&alt != this) {
        if (proxy == 0) {
            if (isLeaf() && alt.proxy) {
                // we are guaranteed to be a Storable
                ((Storable*)this)->copy(*((Storable*)alt.proxy));
            } else {
                setProxy(alt.clone());
            }
        } else {
            if (alt.proxy) {
                if (getCode() == alt.getCode()) {
                    // proxies are guaranteed to be Storable
                    ((Storable*)proxy)->copy(*((Storable*)alt.proxy));
                } else {
                    setProxy(alt.clone());
                }
            } else {
                if (proxy) {
                    delete proxy;
                    proxy = NULL;
                }
                if (alt.isLeaf()) {
                    setProxy(alt.clone());
                }
            }
        }
    }
    return *this;
}

Value::~Value() {
    if (proxy != NULL) {
        delete proxy;
        proxy = NULL;
    }
}

bool Value::isBool() const {
    ok();
    return proxy->isBool();
}

bool Value::isInt() const {
    ok();
    return proxy->isInt();
}

bool Value::isString() const {
    ok();
    return proxy->isString();
}

bool Value::isDouble() const {
    ok();
    return proxy->isDouble();
}

bool Value::isList() const {
    ok();
    return proxy->isList();
}

bool Value::isDict() const {
    ok();
    return proxy->isDict();
}

bool Value::isVocab() const {
    ok();
    return proxy->isVocab();
}

bool Value::isBlob() const {
    ok();
    return proxy->isBlob();
}

bool Value::asBool() const {
    ok();
    return proxy->asBool();
}

int Value::asInt() const {
    ok();
    return proxy->asInt();
}

int Value::asVocab() const {
    ok();
    return proxy->asVocab();
}

double Value::asDouble() const {
    ok();
    return proxy->asDouble();
}

ConstString Value::asString() const {
    ok();
    return proxy->asString();
}

Bottle *Value::asList() const {
    ok();
    return proxy->asList();
}

Property *Value::asDict() const {
    ok();
    return proxy->asDict();
}

Searchable *Value::asSearchable() const {
    ok();
    if (proxy->isDict()) return proxy->asDict();
    return proxy->asList();
}

const char *Value::asBlob() const {
    ok();
    return proxy->asBlob();
}

size_t Value::asBlobLength() const
{
    ok();
    return proxy->asBlobLength();
}

bool Value::read(ConnectionReader& connection) {
    if (proxy) {
        delete proxy;
        proxy = 0;
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

bool Value::write(ConnectionWriter& connection) {
    if (!proxy) {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(0);
        return !connection.isError();
    }
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(1);
    return proxy->write(connection);
}

bool Value::check(const ConstString& key) const {
    ok();
    return proxy->check(key);
}

Value& Value::find(const ConstString& key) const {
    ok();
    return proxy->find(key);
}

Bottle& Value::findGroup(const ConstString& key) const {
    ok();
    return proxy->findGroup(key);
}

bool Value::operator == (const Value& alt) const {
    ok();
    return (*proxy)==alt;
}


bool Value::operator != (const Value& alt) const {
    return !((*this)==alt);
}

void Value::fromString(const char *str) {
    setProxy(makeValue(str));
}

ConstString Value::toString() const {
    ok();
    return proxy->toString();
}

Value *Value::create() const {
    ok();
    return proxy->create();
}

Value *Value::clone() const  {
    ok();
    return proxy->clone();
}

int Value::getCode() const {
    ok();
    return proxy->getCode();
}

bool Value::isNull() const  {
    ok();
    return proxy->isNull();
}

bool Value::isLeaf() const {
    return false;
}

Value *Value::makeInt(int x) {
    return new StoreInt(x);
}


Value *Value::makeDouble(double x) {
    return new StoreDouble(x);
}


Value *Value::makeString(const ConstString& str) {
    return new StoreString(str);
}


Value *Value::makeVocab(int v) {
    return new StoreVocab(v);
}


Value *Value::makeVocab(const ConstString& str) {
    return new StoreVocab(Vocab::encode(str));
}


Value *Value::makeBlob(void *data, int length) {
    String s((char*)data,length);
    return new StoreBlob(s);
}


Value *Value::makeList() {
    return new StoreList();
}


Value *Value::makeList(const char *txt) {
    Value *v = makeList();
    if (v!=NULL) {
        v->asList()->fromString(txt);
    }
    return v;
}


Value *Value::makeValue(const ConstString& txt) {
    Bottle bot(txt);
    if (bot.size()>1) {
        return makeString(txt);
    }
    return bot.get(0).clone();
}


Value& Value::getNullValue() {
    return BottleImpl::getNull();
}


void Value::setProxy(Value *proxy) {
    if (this->proxy!=NULL) {
        delete this->proxy;
        this->proxy = NULL;
    }
    yAssert(proxy!=NULL);
    this->proxy = proxy;
}


void Value::ok() const {
    const Value *op = this;
    if (proxy==NULL) {
        ((Value*)op)->setProxy(makeList());
    }
}




