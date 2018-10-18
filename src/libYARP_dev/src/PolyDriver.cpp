/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

    virtual void report(const SearchReport& report, const char *context) override {
        std::string ctx = context;
        std::string key = report.key;
        std::string prefix;

        prefix = ctx;
        prefix += ".";

        key = prefix + key;
        if (key.substr(0,1)==".") {
            key = key.substr(1,key.length());
        }

        if (!present.check(key)) {
            present.put(key,"present");
            order.addString(key.c_str());
        }

        if (report.isFound) {
            actual.put(key,report.value);
            return;
        }

        if (report.isComment==true) {
            comment.put(key,report.value);
            return;
        }

        if (report.isDefault==true) {
            fallback.put(key,report.value);
            return;
        }
    }

    Bottle getOptions() {
        return order;
    }

    std::string getComment(const char *option) {
        std::string desc = comment.find(option).toString();
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


bool PolyDriver::open(const std::string& txt) {
    Property p;
    p.put("device",txt);
    return open(p);
}


bool PolyDriver::open(yarp::os::Searchable& config) {
    if (isValid()) {
        // already open - should close first
        return false;
    }
    if (system_resource==nullptr) {
        system_resource = new YarpDevMonitor;
    }
    yAssert(system_resource!=nullptr);
    bool removeMonitorAfterwards = false;
    if (config.getMonitor()==nullptr) {
        config.setMonitor(&HELPER(system_resource));
        removeMonitorAfterwards = true;
    }

    //dd = Drivers::factory().open(config);
    coreOpen(config);
    HELPER(system_resource).info.fromString(config.toString());
    if (removeMonitorAfterwards) {
        config.setMonitor(nullptr);
    }
    return isValid();
}


bool PolyDriver::closeMain() {
    bool result = false;
    if (system_resource!=nullptr) {
        int ct = HELPER(system_resource).removeRef();
        if (ct==0) {
            yAssert(system_resource!=nullptr);
            delete &HELPER(system_resource);
            system_resource = nullptr;
            if (dd!=nullptr) {
                result = dd->close();
                delete dd;
                dd = nullptr;
            } else {
                result = true;
            }
        }
        dd = nullptr;
        system_resource = nullptr;
    }
    return result;
}


bool PolyDriver::link(PolyDriver& alt) {
    if (!alt.isValid()) return false;
    if (isValid()) return false;
    dd = alt.dd;
    if (system_resource!=nullptr) {
        int ct = HELPER(system_resource).removeRef();
        if (ct==0) {
            yAssert(system_resource!=nullptr);
            delete &HELPER(system_resource);
        }
    }
    system_resource = alt.system_resource;
    yAssert(dd!=nullptr);
    yAssert(system_resource!=nullptr);
    HELPER(system_resource).addRef();
    return true;
}



PolyDriver::~PolyDriver() {
    closeMain();
    yAssert(dd==nullptr);
    yAssert(system_resource==nullptr);
}



Bottle PolyDriver::getOptions() {
    if (system_resource==nullptr) {
        return Bottle::getNullBottle();
    }
    return HELPER(system_resource).getOptions();
}

std::string PolyDriver::getComment(const char *option) {
    if (system_resource==nullptr) {
        return "";
    }
    return HELPER(system_resource).getComment(option);
}

Value PolyDriver::getDefaultValue(const char *option) {
    if (system_resource==nullptr) {
        return Value::getNullValue();
    }
    return HELPER(system_resource).getDefaultValue(option);
}

Value PolyDriver::getValue(const char *option) {
    if (system_resource==nullptr) {
        return Value::getNullValue();
    }
    return HELPER(system_resource).getValue(option);
}



bool PolyDriver::coreOpen(yarp::os::Searchable& prop) {
    yarp::os::Searchable *config = &prop;
    Property p;
    std::string str = prop.toString();
    Value *part;
    if (prop.check("device",part)) {
        str = part->toString();
    }

    DeviceDriver *driver = nullptr;

    DriverCreator *creator = Drivers::factory().find(str.c_str());
    if (creator!=nullptr) {
        Value *val;
        if (config->check("wrapped",val)&&(creator->getWrapper()!="")) {
            std::string wrapper = creator->getWrapper();
            DriverCreator *wrapCreator =
                Drivers::factory().find(wrapper.c_str());
            if (wrapCreator!=nullptr) {
                p.fromString(config->toString());
                p.unput("wrapped");
                config = &p;
                if (wrapCreator!=creator) {
                    p.put("subdevice",str);
                    p.put("device",wrapper);
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
        // FIXME do not use yarpdev here
        printf("yarpdev: ***ERROR*** could not find device <%s>\n", str.c_str());
        return false;
    }

    if (driver!=nullptr) {
        PolyDriver *manager = creator->owner();
        if (manager!=nullptr) {
            link(*manager);
            return true;
        }

        //printf("yarpdev: parameters are %s\n", config->toString().c_str());
        bool ok = driver->open(*config);
        if (!ok) {
            printf("yarpdev: ***ERROR*** driver <%s> was found but could not open\n", config->find("device").toString().c_str());
            //YARP_DEBUG(Logger::get(),String("Discarding ") + str);
            delete driver;
            //YARP_DEBUG(Logger::get(),String("Discarded ") + str);
            driver = nullptr;
        } else {
            yarp::dev::DeprecatedDeviceDriver *ddd = nullptr;
            driver->view(ddd);
            if(ddd) {
                if(config->check("allow-deprecated-devices")) {
                    yWarning("Device \"%s\" is deprecated. Opening since the \"allow-deprecated-devices\" option was passed in the configuration.", str.c_str());
                } else {
                    yError("Device \"%s\" is deprecated. Pass the \"allow-deprecated-devices\" option in the configuration if you want to open it anyway.", str.c_str());
                    driver->close();
                    delete driver;
                    return false;
                }
            }
            std::string name = creator->getName();
            std::string wrapper = creator->getWrapper();
            std::string code = creator->getCode();
            yInfo("created %s <%s>. See C++ class %s for documentation.",
                  ((name==wrapper)?"wrapper":"device"),
                  name.c_str(),
                  code.c_str());
        }
        dd = driver;
        return true;
    }

    return false;
}


DeviceDriver *PolyDriver::take() {
    // this is not very careful
    DeviceDriver *result = dd;
    dd = nullptr;
    return result;
}

bool PolyDriver::give(DeviceDriver *dd, bool own) {
    close();
    this->dd = dd;
    if (dd!=nullptr) {
        if (system_resource==nullptr) {
            system_resource = new YarpDevMonitor;
        }
        yAssert(system_resource!=nullptr);
        if (!own) {
            HELPER(system_resource).addRef();
        }
    }
    return true;
}
