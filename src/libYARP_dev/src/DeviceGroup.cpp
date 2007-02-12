// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/String.h>
#include <yarp/Logger.h>

#include <yarp/dev/Drivers.h>
#include <yarp/dev/DeviceGroup.h>
#include <yarp/os/Time.h>
#include <stdio.h>

#include <yarp/dev/AudioVisualInterfaces.h>


#include <ace/Vector_T.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


#define HELPER(x) (*((DeviceGroupHelper*)(x)))


class LinkCreator : public DriverCreator {
private:
    ConstString name;
    PolyDriver holding;
public:
    LinkCreator(const char *name, PolyDriver& source) {
        this->name = name;
        holding.link(source);
    }

    virtual ~LinkCreator() {
        holding.close();
    }

    virtual yarp::os::ConstString toString() {
        return name;
    }

    virtual DeviceDriver *create() {
        DeviceDriver *internal;
        holding.view(internal);
        return internal;
    }
    
    virtual yarp::os::ConstString getName() {
        return name;
    }
    
    virtual yarp::os::ConstString getWrapper() {
        return "(link)";
    }

    virtual yarp::os::ConstString getCode() {
        return "LinkCreator";
    }

    virtual PolyDriver *owner() {
        return &holding;
    }

    void close() {
        holding.close();
    }
};


class DeviceGroupHelper {
private:
    ACE_Vector<PolyDriver *> drivers;
    ACE_Vector<ConstString> names;
public:

    void clear() {
        ACE_Vector<PolyDriver *>& lst = drivers;
        for (unsigned int i=0; i<lst.size(); i++) {
            printf("*** Removing %s\n",names[i].c_str());
            Drivers::factory().remove(names[i].c_str());
            //printf("*** removed %s\n",names[i].c_str());
            delete lst[i];
            //printf("*** deleted %s\n",names[i].c_str());
        }
        lst.clear();
        names.clear();
    }

    bool close() {
        printf("*** Device group closing\n");
        clear();
        printf("*** Device group closed\n");
        return true;
    }

    ~DeviceGroupHelper() {
        clear();
    }

    bool add(const char *name, yarp::os::Searchable& config) {
        //printf("ADDING %s\n", config.toString().c_str());
        PolyDriver *pd = new PolyDriver();
        YARP_ASSERT (pd!=NULL);
        bool result = pd->open(config);
        if (!result) {
            delete pd;
            return false;
        }
        drivers.push_back(pd);
        names.push_back(ConstString(name));
        Drivers::factory().add(new LinkCreator(name,*pd));
        return true;
    }

};




bool DeviceGroup::open(yarp::os::Searchable& config) {
    if (implementation==NULL) {
        implementation = new DeviceGroupHelper;
    }
    if (implementation==NULL) {
        printf("Out of memory\n");
        return false;
    }

    if (config.check("part","a list of section names, with each section containing a device")) {
        Bottle bot = config.findGroup("part").tail();
        printf("Hmm, seems to be an assembly\n");
        printf("Assembly of: %s\n", bot.toString().c_str());
        for (int i=0; i<bot.size(); i++) {
            ConstString name = bot.get(i).asString();
            printf("  %s -> %s\n", name.c_str(), 
                   config.findGroup(name).toString().c_str());
            bool result = HELPER(implementation).add(name,
                                                     config.findGroup(name));
            if (!result) {
                HELPER(implementation).close();
                return false;
            }
        }
        return true;
    }

    bool ok = open("source",source,config,
                   "device to read from (string or nested properties)");
    if (!ok) return false;
    ok = open("sink",sink,config,"device to write to (string or nested properties)");
    if (!ok) {
        source.close();
        return false;
    }
    return true;
}


bool DeviceGroup::open(const char *key, PolyDriver& poly, 
                      yarp::os::Searchable& config, const char *comment) {
    
    Value *name;
    if (config.check(key,name,comment)) {
        if (name->isString()) {
            // maybe user isn't doing nested configuration
            yarp::os::Property p;
            p.setMonitor(config.getMonitor(),
                         name->toString().c_str()); // pass on any monitoring
            p.fromString(config.toString());
            p.put("device",name->toString());
            p.unput("subdevice");
            p.unput("wrapped");
            poly.open(p);
        } else {
            Bottle subdevice = config.findGroup(key).tail();
            poly.open(subdevice);
        }
        if (!poly.isValid()) {
            printf("cannot make <%s>\n", name->toString().c_str());
            return false;
        }
    } else {
        printf("\"--%s <name>\" not set\n", key);
        return false;
    }
    return true;
}


bool DeviceGroup::closeMain() {
    printf("Devices closing\n");
    HELPER(implementation).close();
    source.close();
    sink.close();
    return true;
}


bool DeviceGroup::updateService() {
    if (source.isValid()&&sink.isValid()) {
        IFrameGrabberImage *imgSource;
        IFrameWriterImage *imgSink;
        source.view(imgSource);
        sink.view(imgSink);
        if (imgSource!=NULL&&imgSink!=NULL) {
            ImageOf<PixelRgb> tmp;
            imgSource->getImage(tmp);
            imgSink->putImage(tmp);
            printf("piped %dx%d image\n", tmp.width(), tmp.height());
        } else {
            printf("Don't know how to pipe between these devices.\n");
            printf("Piping is very limited at the moment.\n");
            printf("You're probably better off writing some short custom code.\n");
            return false;
        }
    } else {
        //printf("Device aggregation in operation...\n");
        Time::delay(1);
    }
    return true;
}


DeviceGroup::~DeviceGroup() {
    if (implementation!=NULL) {
        delete &HELPER(implementation);
        implementation = NULL;
    }
}


