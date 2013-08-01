// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Log.h>
#include <yarp/os/Property.h>
#include <yarp/dev/PolyDriver.h>

using namespace yarp::os;
using namespace yarp::dev;

class YarpDevMonitor : public SearchMonitor {
private:
    Property comment, fallback, present, actual;
    Bottle order;
    int count;

public:
    Property info;

    YarpDevMonitor() {
        count = 1;
    }

    virtual void report(const SearchReport& report, const char *context) {
        ConstString ctx = context;
        ConstString key = report.key.c_str();
        ConstString prefix = "";

        prefix = ctx;
        prefix += ".";

        key = prefix + key;
        if (key.substr(0,1)==".") {
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

    void addRef() {
        count++;
    }

    int removeRef() {
        count--;
        return count;
    }

    int getRef() {
        return count;
    }
};


#define HELPER(x) (*((YarpDevMonitor*)(x)))


bool PolyDriver::open(const ConstString& txt) {
    Property p;
    p.put("device",txt);
    return open(p);
}


bool PolyDriver::open(yarp::os::Searchable& config) {
    if (isValid()) {
        YARP_LOG_DEBUG("PolyDriver already open");
        // already open - should close first
        return false;
    }
    YARP_LOG_DEBUG("PolyDriver opening...");
    if (system_resource==NULL) {
        system_resource = new YarpDevMonitor;
    }
    YARP_ASSERT(system_resource!=NULL);
    bool removeMonitorAfterwards = false;
    if (config.getMonitor()==NULL) {
        config.setMonitor(&HELPER(system_resource));
        removeMonitorAfterwards = true;
    }

    YARP_LOG_DEBUG("PolyDriver calling factory...");
    //dd = Drivers::factory().open(config);
    coreOpen(config);
    HELPER(system_resource).info.fromString(config.toString());
    YARP_LOG_DEBUG("PolyDriver opened.");
    if (removeMonitorAfterwards) {
        config.setMonitor(NULL);        
    }
    return isValid();
}


bool PolyDriver::closeMain() {
    bool result = false;
    if (system_resource!=NULL) {
        YARP_LOG_DEBUG(ConstString("PolyDriver closing ") +
                       HELPER(system_resource).info.toString().c_str());
        int ct = HELPER(system_resource).removeRef();
        if (ct==0) {
            YARP_ASSERT(system_resource!=NULL);
            delete &HELPER(system_resource);
            system_resource = NULL;
            if (dd!=NULL) {
                result = dd->close();
                YARP_LOG_DEBUG("PolyDriver closed.");
                delete dd;
                dd = NULL;
            }
            result = true;
        } else {
            YARP_LOG_DEBUG("PolyDriver still in use.");
        }
        dd = NULL;
        system_resource = NULL;
    }
    return result;
}


bool PolyDriver::link(PolyDriver& alt) {
    if (!alt.isValid()) return false;
    if (isValid()) return false;
    dd = alt.dd;
    system_resource = alt.system_resource;
    YARP_ASSERT(dd!=NULL);
    YARP_ASSERT(system_resource!=NULL);
    HELPER(system_resource).addRef();
    return true;
}



PolyDriver::~PolyDriver() {
    closeMain();
    YARP_ASSERT(dd==NULL);
    YARP_ASSERT(system_resource==NULL);
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



bool PolyDriver::coreOpen(yarp::os::Searchable& prop) {
    yarp::os::Searchable *config = &prop;
    Property p;
    ConstString str = prop.toString();
    Value *part;
    if (prop.check("device",part)) {
        str = part->toString().c_str();
    }
    Bottle bot(str.c_str());
    if (bot.size()>1) {
        // this wasn't a device name, but some codes -- rearrange
        p.fromString(str.c_str());
        str = p.find("device").asString().c_str();
        config = &p;
    }
    YARP_LOG_DEBUG(ConstString("Drivers::open starting for ") + str);

    DeviceDriver *driver = NULL;

    DriverCreator *creator = Drivers::factory().find(str.c_str());
    if (creator!=NULL) {
        Value *val;
        if (config->check("wrapped",val)&&(creator->getWrapper()!="")) {
            ConstString wrapper = creator->getWrapper();
            DriverCreator *wrapCreator =
                Drivers::factory().find(wrapper.c_str());
            if (wrapCreator!=NULL) {
                p.fromString(config->toString());
                p.unput("wrapped");
                config = &p;
                if (wrapCreator!=creator) {
                    p.put("subdevice",str.c_str());
                    p.put("device",wrapper.c_str());
                    p.setMonitor(prop.getMonitor(),
                                 wrapper.c_str()); // pass on any monitoring
                    driver = wrapCreator->create();
                    creator = wrapCreator;
                } else {
                    // already wrapped
                    driver = creator->create();
                }
            }
        } else {
            driver = creator->create();
        }
    } else {
        printf("yarpdev: ***ERROR*** could not find device <%s>\n", str.c_str());
    }

    YARP_LOG_DEBUG(ConstString("Drivers::open started for ") + str);

    if (driver!=NULL) {
        PolyDriver *manager = creator->owner();
        if (manager!=NULL) {
            link(*manager);
            return true;
        }

        //printf("yarpdev: parameters are %s\n", config->toString().c_str());
        YARP_LOG_DEBUG(ConstString("Drivers::open config for ") + str);
        bool ok = driver->open(*config);
        YARP_LOG_DEBUG(ConstString("Drivers::open configed for ") + str);
        if (!ok) {
            printf("yarpdev: ***ERROR*** driver <%s> was found but could not open\n", config->find("device").toString().c_str());
            //YARP_DEBUG(Logger::get(),String("Discarding ") + str);
            delete driver;
            //YARP_DEBUG(Logger::get(),String("Discarded ") + str);
            driver = NULL;
        } else {
            if (creator!=NULL) {
                ConstString name = creator->getName();
                ConstString wrapper = creator->getWrapper();
                ConstString code = creator->getCode();
                if (yarp_show_info()) {
                    ConstString msg = ConstString("created ") +
                        ((name==wrapper)?"wrapper":"device") +
                        " <" + name + ">. See C++ class " +
                        code + " for documentation.";
                    YARP_LOG_INFO(msg);
                }
            }
        }
        dd = driver;
        return true;
    }

    return false;
}


DeviceDriver *PolyDriver::take() {
    // this is not very careful
    DeviceDriver *result = dd;
    dd = NULL;
    return result;
}

bool PolyDriver::give(DeviceDriver *dd, bool own) {
    close();
    this->dd = dd;
    if (dd!=NULL) {
        if (system_resource==NULL) {
            system_resource = new YarpDevMonitor;
        }
        YARP_ASSERT(system_resource!=NULL);
        if (!own) {
            HELPER(system_resource).addRef();
        }
    }
    return true;
}



