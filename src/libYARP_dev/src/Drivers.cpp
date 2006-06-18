// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/dev/Drivers.h>
#include <yarp/String.h>
#include <yarp/Logger.h>

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
            s += delegates[i]->toString();
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
    BottleBit *part;
    if (prop.check("device",part)) {
        str = part->toString().c_str();
    }
    Bottle bot(str.c_str());
    if (bot.size()>1) {
        // this wasn't a device name, but some codes -- rearrange
        p.fromString(str.c_str());
        str = p.get("device").asString().c_str();
        config = &p;
    }

    DeviceDriver *driver = NULL;

    DriverCreator *creator = find(str.c_str());
    if (creator!=NULL) {
        BottleBit *val;
        if (config->check("wrapped",val)) {
            String wrapper = creator->getWrapper().c_str();
            DriverCreator *wrapCreator = find(wrapper.c_str());
            if (wrapCreator!=NULL) {
                p.fromString(config->toString());
                p.unput("wrapped");
                config = &p;
                if (wrapCreator!=creator) {
                    p.put("subdevice",str.c_str());
                    p.put("device",wrapper.c_str());
                    driver = wrapCreator->create();
                } else {
                    // already wrapped
                    driver = creator->create();
                }
            }
        } else {
            driver = creator->create();
        }
    } else {
        printf("Could not find device <%s>\n", str.c_str());
    }

    if (driver!=NULL) {
        bool ok = driver->open(*config);
        if (!ok) {
            printf("Driver <%s> was found but could not open\n", str.c_str());
            delete driver;
            driver = NULL;
        }
        return driver;
    }
    
    return NULL;
}



// defined in PopulateDrivers.cpp:
//   DeviceDriver *Drivers::init()


