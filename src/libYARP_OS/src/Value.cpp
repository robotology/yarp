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
using namespace yarp::os::impl;

Value *Value::makeInt(int x) {
    return new StoreInt(x);
}


Value *Value::makeDouble(double x) {
    return new StoreDouble(x);
}


Value *Value::makeString(const char *str) {
    return new StoreString(str);
}


Value *Value::makeVocab(int v) {
    return new StoreVocab(v);
}


Value *Value::makeVocab(const char *str) {
    return new StoreVocab(Vocab::encode(str));
}


Value *Value::makeBlob(void *data, int length) {
    String s;
    YARP_STRSET(s,(char*)data,length,0);
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


Value *Value::makeValue(const char *txt) {
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
    YARP_ASSERT(proxy!=NULL);
    this->proxy = proxy;
}


void Value::ok() const {
    const Value *op = this;
    if (proxy==NULL) {
        ((Value*)op)->setProxy(makeList());
    }
}


Value::Value(const Value& alt) : Portable(), Searchable() {
    proxy = 0;
    setProxy(alt.clone());
}


const Value& Value::operator = (const Value& alt) {
    if (proxy==0) {
        if (isLeaf()) {
            // we are guaranteed to be a Storable
            ((Storable*)this)->copy(*((Storable*)alt.proxy));
        } else {
            setProxy(alt.clone());
        }
    } else {
        if (alt.proxy) {
            if (getCode()==alt.getCode()) {
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
        }
    }
    return *this;
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


Searchable *Value::asSearchable() const {
    ok(); 
    if (proxy->isDict()) return proxy->asDict(); 
    return proxy->asList();
}
