// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2008 RobotCub Consortium, Arjan Gijsberts
 * Authors: Paul Fitzpatrick, Arjan Gijsberts
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/impl/BottleImpl.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/NetType.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/DummyConnector.h>

using namespace yarp::os::impl;
using namespace yarp::os;

class NullBottle : public Bottle {
public:
    NullBottle() {
        setReadOnly(true);
    }

    virtual bool isNull() const    { return true; }

    static NullBottle *bottleNull;
};

NullBottle *NullBottle::bottleNull = NULL;


// implementation is a BottleImpl
#define HELPER(x) (*((BottleImpl*)(x)))

Bottle::Bottle() {
    implementation = new BottleImpl;
    invalid = false;
    ro = false;
    yAssert(implementation!=NULL);
}

Bottle::Bottle(const Bottle& bottle) : Portable(), Searchable() {
    implementation = new BottleImpl;
    invalid = false;
    ro = false;
    yAssert(implementation!=NULL);
    copy(bottle);
}

const Bottle& Bottle::operator = (const Bottle& bottle) {
    edit();
    copy(bottle);
    return *this;
}


Bottle::Bottle(const ConstString& text) {
    implementation = new BottleImpl;
    invalid = false;
    ro = false;
    yAssert(implementation!=NULL);
    fromString(text);
}

Bottle::~Bottle() {
    if (implementation!=NULL) {
        delete &HELPER(implementation);
        implementation = NULL;
    }
}

void Bottle::clear() {
    edit();
    invalid = false;
    HELPER(implementation).clear();
}

void Bottle::addInt(int x) {
    edit();
    HELPER(implementation).addInt(x);
}

void Bottle::addVocab(int x) {
    edit();
    HELPER(implementation).addVocab(x);
}

void Bottle::addDouble(double x) {
    edit();
    HELPER(implementation).addDouble(x);
}

void Bottle::addString(const char *str) {
    edit();
    HELPER(implementation).addString(str);
}

void Bottle::addString(const ConstString& str) {
    edit();
    HELPER(implementation).addString(str);
}

Bottle& Bottle::addList() {
    edit();
    return HELPER(implementation).addList();
}

Property& Bottle::addDict() {
    edit();
    return HELPER(implementation).addDict();
}

Value Bottle::pop() {
    edit();
    Storable* stb = HELPER(implementation).pop();
    Value val(*stb);
    // here we take responsibility for deallocation of the Storable instance
    delete stb;
    return val;
}

int Bottle::getInt(int index) {
    return HELPER(implementation).getInt(index);
}

ConstString Bottle::getString(int index) {
    return ConstString(HELPER(implementation).getString(index).c_str());
}

double Bottle::getDouble(int index) {
    return HELPER(implementation).getDouble(index);
}

Bottle *Bottle::getList(int index) {
    return HELPER(implementation).getList(index);
}

Value& Bottle::get(int index) const {
    return HELPER(implementation).get(index);
}


bool Bottle::isInt(int index) {
    return HELPER(implementation).isInt(index);
}

bool Bottle::isDouble(int index) {
    return HELPER(implementation).isDouble(index);
}

bool Bottle::isString(int index) {
    return HELPER(implementation).isString(index);
}

bool Bottle::isList(int index) {
    return HELPER(implementation).isList(index);
}

void Bottle::fromString(const ConstString& text) {
    edit();
    invalid = false;
    HELPER(implementation).fromString(text.c_str());
}

ConstString Bottle::toString() const {
    return ConstString(HELPER(implementation).toString().c_str());
}

void Bottle::fromBinary(const char *text, int len) {
    edit();
    HELPER(implementation).fromBinary(text,len);
}


const char *Bottle::toBinary(size_t *size) {
    if (size!=NULL) {
        *size = HELPER(implementation).byteCount();
    }
    return HELPER(implementation).getBytes();
}



bool Bottle::write(ConnectionWriter& writer) {
    return HELPER(implementation).write(writer);
}


void Bottle::onCommencement() {
    HELPER(implementation).onCommencement();
}

bool Bottle::read(ConnectionReader& reader) {
    edit();
    return HELPER(implementation).read(reader);
}

int Bottle::size() const {
    return (int)HELPER(implementation).size();
}

void Bottle::hasChanged() {
    return HELPER(implementation).hasChanged();
}

void Bottle::specialize(int subCode) {
    HELPER(implementation).specialize(subCode);
}


int Bottle::getSpecialization() {
    return HELPER(implementation).getSpecialization();
}


void Bottle::setNested(bool nested) {
    HELPER(implementation).setNested(nested);
}



void Bottle::copy(const Bottle& alt, int first, int len) {
    edit();
    if (alt.isNull()) {
        clear();
        invalid = true;
        return;
    }
    HELPER(implementation).copyRange(HELPER(alt.implementation),
                                     first,
                                     len);
}

Value& Bottle::findGroupBit(const ConstString& key) const {
    for (int i=0; i<size(); i++) {
        Value *org = &(get(i));
        Value *cursor = org;
        if (cursor->isList()) {
            cursor = &(cursor->asList()->get(0));
        }
        if (key==cursor->toString()) {
            return *org;
        }
    }
    // return invalid object
    return get(-1);
}


Value& Bottle::findBit(const ConstString& key) const {
    for (int i=0; i<size(); i++) {
        Value *org = &(get(i));
        Value *cursor = org;
        bool nested = false;
        if (cursor->isList()) {
            Bottle *bot = cursor->asList();
            cursor = &(bot->get(0));
            nested = true;
        }
        if (key==cursor->toString()) {
            if (nested) {
                return org->asList()->get(1);
            }
            if (getMonitor()!=NULL) {
                SearchReport report;
                report.key = key;
                report.isFound = true;
                if (size()==2) {
                    report.value = get(i+1).toString();
                } else {
                }
                reportToMonitor(report);
            }
            return get(i+1);
        }
    }
    // return invalid object
    if (getMonitor()!=NULL) {
        SearchReport report;
        report.key = key;
        reportToMonitor(report);
    }
    return get(-1);
}


bool Bottle::check(const ConstString& key) const {
    Bottle& val = findGroup(key);
    if (!val.isNull())
        return true;
    Value& val2 = find(key);
    return !val2.isNull();
}


Value& Bottle::find(const ConstString& key) const {
    Value& val = findBit(key);

    if (getMonitor()!=NULL) {
        SearchReport report;
        report.key = key;
        report.isFound = !val.isNull();
        report.value = val.toString();
        reportToMonitor(report);
    }

    return val;
}


Bottle& Bottle::findGroup(const ConstString& key) const {
    Value& bb = findGroupBit(key);

    if (getMonitor()!=NULL) {
        SearchReport report;
        report.key = key;
        report.isGroup = true;
        if (bb.isList()) {
            report.isFound = true;
            report.value = bb.toString();
        }
        reportToMonitor(report);
        if (bb.isList()) {
            String context = getContext().c_str();
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


Bottle *Bottle::clone() {
    Bottle *b = new Bottle();
    yAssert(b!=NULL);
    b->copy(*this);
    return b;
}

void Bottle::add(Value *value) {
    edit();
    HELPER(implementation).addBit(value);
}


void Bottle::add(const Value& value) {
    edit();
    HELPER(implementation).addBit(value);
}


Bottle& Bottle::getNullBottle() {
    if (!NullBottle::bottleNull) NullBottle::bottleNull = new NullBottle();
    return *NullBottle::bottleNull;
}


void Bottle::fini() {
    if (NullBottle::bottleNull) {
        delete NullBottle::bottleNull;
        NullBottle::bottleNull = NULL;
    }
}

bool Bottle::operator == (const Bottle& alt) {
    return String(toString().c_str()) == alt.toString().c_str();
}


bool Bottle::write(PortReader& reader, bool textMode) {
    DummyConnector con;
    con.setTextMode(textMode);
    write(con.getWriter());
    return reader.read(con.getReader());
}

bool Bottle::read(PortWriter& writer, bool textMode) {
    edit();
    DummyConnector con;
    con.setTextMode(textMode);
    writer.write(con.getWriter());
    return read(con.getReader());
}

bool Bottle::isNull() const {
    return invalid;
}

bool Bottle::operator!=(const Bottle& alt) {
    return !((*this)==alt);
}

void Bottle::append(const Bottle& alt) {
    edit();
    for (int i=0; i<alt.size(); i++) {
        add(alt.get(i));
    }
}

Bottle Bottle::tail() const {
    Bottle b;
    if (isNull()) return *this;
    b.copy(*this,1,size()-1);
    return b;
}

Bottle *Bottle::create() {
    return new Bottle();
}

bool Bottle::isList() {
    return true;
}


ConstString Bottle::toString(int x) {
    return NetType::toString(x);
}


void Bottle::edit() {
    if (ro) {
        yFatal("Attempted to modify the null bottle");
    }
    if (invalid) {
        invalid = false;
    }
}
