// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/String.h>
#include <yarp/Logger.h>
#include <yarp/dev/Drivers.h>

#include <ace/OS.h>
#include <ace/Vector_T.h>


using namespace yarp::os;
using namespace yarp::dev;


Drivers Drivers::instance;


class DriversHelper {
public:
    ACE_Vector<DriverCreator *> delegates;

    ~DriversHelper() {
        for (unsigned int i=0; i<delegates.size(); i++) {
            delete delegates[i];
        }
        delegates.clear();
    }

    ConstString toString() {
        yarp::String s;
        for (unsigned int i=0; i<delegates.size(); i++) {
            ConstString name = delegates[i]->getName();
            ConstString wrapper = delegates[i]->getWrapper();
            s += "Device \"";
            s += delegates[i]->getName().c_str();
            s += "\"";
            s += ",";
            s += " C++ class ";
            s += delegates[i]->getCode().c_str();
            s += ", ";
            if (wrapper=="") {
                s += "has no network wrapper";
            } else if (wrapper!=name) {
                s += "wrapped by \"";
                s += delegates[i]->getWrapper().c_str();
                s += "\"";
            } else {
                s += "is a network wrapper.";
            }
            s += "\n";
        }
        return ConstString(s.c_str());
    }

    void add(DriverCreator *creator) {
        if (creator!=NULL) {
            delegates.push_back(creator);
        }
    }

    DriverCreator *find(const char *name) {
        for (unsigned int i=0; i<delegates.size(); i++) {
            yarp::String s = delegates[i]->toString().c_str();
            if (s==name) {
                return delegates[i];
            }
        }
        return NULL;
    }
};

#define HELPER(x) (*(((DriversHelper*)(x))))

Drivers::Drivers() {
    implementation = new DriversHelper;
    YARP_ASSERT(implementation!=NULL);
    init();
}


Drivers::~Drivers() {
    if (implementation!=NULL) {
        delete &HELPER(implementation);
        implementation = NULL;
    }
}

yarp::os::ConstString Drivers::toString() {
    return HELPER(implementation).toString();
}

void Drivers::add(DriverCreator *creator) {
    HELPER(implementation).add(creator);
}


DriverCreator *Drivers::find(const char *name) {
    return HELPER(implementation).find(name);
}


DeviceDriver *Drivers::open(yarp::os::Searchable& prop) {
    yarp::os::Searchable *config = &prop;
    Property p;
    String str = prop.toString().c_str();
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
    YARP_DEBUG(Logger::get(),String("Drivers::open starting for ") + str);

    DeviceDriver *driver = NULL;

    DriverCreator *creator = find(str.c_str());
    if (creator!=NULL) {
        Value *val;
        if (config->check("wrapped",val)&&(creator->getWrapper()!="")) {
            String wrapper = creator->getWrapper().c_str();
            DriverCreator *wrapCreator = find(wrapper.c_str());
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

    YARP_DEBUG(Logger::get(),String("Drivers::open started for ") + str);

    if (driver!=NULL) {
        //printf("yarpdev: parameters are %s\n", config->toString().c_str());
        YARP_DEBUG(Logger::get(),String("Drivers::open config for ") + str);
        bool ok = driver->open(*config);
        YARP_DEBUG(Logger::get(),String("Drivers::open configed for ") + str);
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
                printf("yarpdev: created %s <%s>.  See C++ class %s for documentation.\n",
                       (name==wrapper)?"wrapper":"device",
                       name.c_str(), code.c_str());
            }
        }
        return driver;
    }
    
    return NULL;
}



// defined in PopulateDrivers.cpp:
//   DeviceDriver *Drivers::init()


