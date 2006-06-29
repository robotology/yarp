// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/BottleImpl.h>
#include <yarp/Logger.h>
#include <yarp/IOException.h>

#include <yarp/os/Bottle.h>

using namespace yarp;
using namespace yarp::os;

class NullBottle : public Bottle {
public:
    virtual bool isNull()    { return true; }

    static NullBottle bottleNull;
};

NullBottle NullBottle::bottleNull;


// implementation is a BottleImpl
#define HELPER(x) (*((BottleImpl*)(x)))

Bottle::Bottle() {
    implementation = new BottleImpl;
    YARP_ASSERT(implementation!=NULL);
}

Bottle::Bottle(const Bottle& bottle) {
    implementation = new BottleImpl;
    YARP_ASSERT(implementation!=NULL);
    fromString(bottle.toString().c_str());
}

const Bottle& Bottle::operator = (const Bottle& bottle) {
    fromString(bottle.toString().c_str());
    return *this;
}


Bottle::Bottle(const char *text) {
    implementation = new BottleImpl;
    YARP_ASSERT(implementation!=NULL);
    fromString(text);
}

Bottle::~Bottle() {
    if (implementation!=NULL) {
        delete &HELPER(implementation);
        implementation = NULL;
    }
}

void Bottle::clear() {
    HELPER(implementation).clear();
}

void Bottle::addInt(int x) {
    HELPER(implementation).addInt(x);
}

void Bottle::addVocab(int x) {
    HELPER(implementation).addVocab(x);
}

void Bottle::addDouble(double x) {
    HELPER(implementation).addDouble(x);
}

void Bottle::addString(const char *str) {
    HELPER(implementation).addString(str);
}

Bottle& Bottle::addList() {
    return HELPER(implementation).addList();
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

void Bottle::fromString(const char *text) {
    HELPER(implementation).fromString(text);
}

ConstString Bottle::toString() const {
    return ConstString(HELPER(implementation).toString().c_str());
}

void Bottle::fromBinary(const char *text, int len) {
    HELPER(implementation).fromBinary(text,len);
}



bool Bottle::write(ConnectionWriter& writer) {
    bool result = false;
    try {
        //ConnectionWriterAdapter adapter(writer);
        result = HELPER(implementation).write(writer);
        //result = true;
    } catch (IOException e) {
        YARP_DEBUG(Logger::get(), e.toString() + " <<< Bottle::write saw this");
        // leave result false
    }
    return result;
}


bool Bottle::read(ConnectionReader& reader) {
    bool result = false;
    try {
        //ConnectionReaderAdapter adapter(reader);
        result = HELPER(implementation).read(reader);
    } catch (IOException e) {
        YARP_DEBUG(Logger::get(), e.toString() + " <<< Bottle::read saw this");
        // leave result false
    }
    return result;
}

int Bottle::size() const {
    return HELPER(implementation).size();
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
    HELPER(implementation).copyRange(HELPER(alt.implementation),
                                     first,
                                     len);
}

Value& Bottle::findGroupBit(const char *key) {
    for (int i=0; i<size(); i++) {
        Value *org = &(get(i));
        Value *cursor = org;
        if (cursor->isList()) {
            cursor = &(cursor->asList()->get(0));
        }
        if (String(key)==cursor->toString().c_str()) {
            return *org;
        }
    }
    // return invalid object
    return get(-1);
}


Value& Bottle::findValue(const char *key) {
    for (int i=0; i<size(); i++) {
        Value *org = &(get(i));
        Value *cursor = org;
        bool nested = false;
        if (cursor->isList()) {
            cursor = &(cursor->asList()->get(0));
            nested = true;
        }
        if (String(key)==cursor->toString().c_str()) {
            if (nested) {
                return org->asList()->get(1);
            }
            return get(i+1);
        }
    }
    // return invalid object
    return get(-1);
}

Bottle& Bottle::findGroup(const char *key) {
    Value& bb = findGroupBit(key);
    if (bb.isList()) {
        return *(bb.asList());
    }
    return getNull();
}


Bottle *Bottle::clone() {
    Bottle *b = new Bottle();
    YARP_ASSERT(b!=NULL);
    b->copy(*this);
    return b;
}

void Bottle::add(Value *value) {
    HELPER(implementation).addBit(value);
}


void Bottle::add(Value& value) {
    HELPER(implementation).addBit(value);
}


bool Value::operator == (const Value& alt) {
    return String(toString().c_str()) == alt.toString().c_str();
}


Bottle& Bottle::getNull() {
    return NullBottle::bottleNull;
}


bool Bottle::operator == (const Bottle& alt) {
    return String(toString().c_str()) == alt.toString().c_str();
}


bool Searchable::check(const char *txt, Value *& result) {
    Value& bit = find(txt);
    bool ok = !(bit.isNull());
    if (ok) {
        result = &bit;
    }
    return ok;
}
