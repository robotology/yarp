/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_TYPE_H
#define YARP_OS_TYPE_H

#include <yarp/os/Searchable.h>
#include <string>

namespace yarp {
    namespace os {
        class Type;
        class Property;
    }
}

class YARP_OS_API yarp::os::Type {
public:
    Type();

    Type(const Type& alt);

    virtual ~Type();

    const Type& operator =(const Type& alt);

    static Type byName(const char *name) {
        Type t;
        t.name = name;
        return t;
    }

    static Type byName(const char *name, const char *name_on_wire) {
        Type t;
        t.name = name;
        t.name_on_wire = name_on_wire;
        return t;
    }

    static Type byNameOnWire(const char *name_on_wire) {
        Type t;
        t.name = "yarp/bottle";
        t.name_on_wire = name_on_wire;
        return t;
    }

    static Type anon() {
        return Type();
    }

    std::string getName() const {
        return name;
    }

    std::string getNameOnWire() const {
        return name_on_wire;
    }

    bool hasName() const {
        return name!="";
    }

    bool isValid() const {
        return hasName();
    }

    std::string toString() const {
        if (name_on_wire!="") {
            return name + ":" + name_on_wire;
        }
        if (name!="") return name;
        return "null";
    }

    const Searchable& readProperties() const;

    Property& writeProperties();

    Type& addProperty(const char *key, const Value& val);

private:
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) name;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) name_on_wire;
    Property *prop;
};

#endif // YARP_OS_TYPE_H
