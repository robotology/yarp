// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/String.h>
#include <yarp/Logger.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Property.h>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp;



class YarpDevMonitor : public SearchMonitor {
private:
    Property comment, fallback, present, actual;
    Bottle order;

public:
    YarpDevMonitor() {
    }

    virtual void report(const SearchReport& report, const char *context) {
        String ctx = context;
        String key = report.key.c_str();
        String prefix = "";

        prefix = ctx;
        prefix += ".";

        key = prefix + key;
        if (key.substr(0,1)=='.') {
            key = key.substr(1,key.length());
        }

        if (!present.check(key.c_str())) {
            present.put(key.c_str(),"present");
            order.addString(key.c_str());
        }

        if (report.isFound) {
            actual.put(key.c_str(),report.value);
            return;
        }

        if (report.isComment==true) {
            comment.put(key.c_str(),report.value);
            return;
        }

        if (report.isDefault==true) {
            fallback.put(key.c_str(),report.value);
            return;
        }
    }

    Bottle getOptions() {
        return order;
    }

    ConstString getComment(const char *option) {
        ConstString desc = comment.find(option).toString();
        return desc;
    }

    Value getDefaultValue(const char *option) {
        return fallback.find(option);
    }

    Value getValue(const char *option) {
        return actual.find(option);
    }
};


#define HELPER(x) (*((YarpDevMonitor*)(x)))


bool PolyDriver::open(const char *txt) {
    Property p;
    p.put("device",txt);
    return open(p);
}


bool PolyDriver::open(yarp::os::Searchable& config) {
    if (isValid()) {
        YARP_DEBUG(Logger::get(),"PolyDriver already open");
        // already open - should close first
        return false;
    }
    YARP_DEBUG(Logger::get(),"PolyDriver opening...");
    if (system_resource==NULL) {
        system_resource = new YarpDevMonitor;
    }
    YARP_ASSERT(system_resource!=NULL);
    if (config.getMonitor()==NULL) {
        config.setMonitor(&HELPER(system_resource));
    }
    YARP_DEBUG(Logger::get(),"PolyDriver calling factory...");
    dd = Drivers::factory().open(config);
    YARP_DEBUG(Logger::get(),"PolyDriver opened.");
    return isValid();
}


PolyDriver::~PolyDriver() {
    if (dd!=0 /*NULL*/) {
        dd->close();
        delete dd;
        dd = 0 /*NULL*/;
    }
    if (system_resource!=NULL) {
        delete &HELPER(system_resource);
        system_resource = NULL;
    }
}



Bottle PolyDriver::getOptions() {
    if (system_resource==NULL) {
        return Bottle::getNullBottle();
    }
    return HELPER(system_resource).getOptions();
}

ConstString PolyDriver::getComment(const char *option) {
    if (system_resource==NULL) {
        return "";
    }
    return HELPER(system_resource).getComment(option);
}

Value PolyDriver::getDefaultValue(const char *option) {
    if (system_resource==NULL) {
        return Value::getNullValue();
    }
    return HELPER(system_resource).getDefaultValue(option);
}

Value PolyDriver::getValue(const char *option) {
    if (system_resource==NULL) {
        return Value::getNullValue();
    }
    return HELPER(system_resource).getValue(option);
}

