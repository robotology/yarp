// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

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

Type::~Type() {
    if (prop) {
        delete prop;
        prop = NULL;
    }
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

