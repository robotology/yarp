/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Type.h>
#include <yarp/os/Property.h>
#include <yarp/os/Log.h>

using namespace yarp::os;

Type::Type() {
    prop = YARP_NULLPTR;
}

Type::Type(const Type& alt) {
    prop = YARP_NULLPTR;
    name = alt.name;
    name_on_wire = alt.name_on_wire;
    if (alt.prop) {
        writeProperties();
        *prop = *(alt.prop);
    }
}

Type::~Type() {
    if (prop) {
        delete prop;
        prop = YARP_NULLPTR;
    }
}

const Type& Type::operator =(const Type& alt) {
    name = alt.name;
    name_on_wire = alt.name_on_wire;
    if (alt.prop) {
        writeProperties();
        *prop = *(alt.prop);
    } else if (prop) {
        delete prop;
        prop = YARP_NULLPTR;
    }
    return *this;
}

const Searchable& Type::readProperties() const {
    if (!prop) return Bottle::getNullBottle();
    return *prop;
}

Property& Type::writeProperties() {
    if (!prop) {
        prop = new Property();
        yAssert(prop);
    }
    return *prop;
}

Type& Type::addProperty(const char *key, const Value& val) {
    writeProperties();
    prop->put(key,val);
    return *this;
}

