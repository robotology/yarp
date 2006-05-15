// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/BottleImpl.h>
#include <yarp/Logger.h>
#include <yarp/IOException.h>

#include <yarp/os/Bottle.h>

using namespace yarp;
using namespace yarp::os;

// implementation is a BottleImpl
#define HELPER(x) (*((BottleImpl*)(x)))

Bottle::Bottle() {
    implementation = new BottleImpl;
    YARP_ASSERT(implementation!=NULL);
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

ConstString Bottle::toString() {
    return ConstString(HELPER(implementation).toString().c_str());;
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

int Bottle::size() {
    return HELPER(implementation).size();
}


