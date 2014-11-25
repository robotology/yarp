// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/DriverLinkCreator.h>
#include <yarp/dev/DeviceGroup.h>

#include <yarp/dev/AudioVisualInterfaces.h>

#include <yarp/os/impl/PlatformVector.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


#define HELPER(x) (*((DeviceGroupHelper*)(x)))


class DeviceGroupHelper {
private:
    PlatformVector<PolyDriver *> drivers;
    PlatformVector<ConstString> names;
    PlatformVector<bool> needDrive;
    Semaphore mutex;
public:
    bool needDriveSummary;

    DeviceGroupHelper() : mutex(1) {
        needDriveSummary = false;
    }

    void clear() {
        mutex.wait();
        PlatformVector<PolyDriver *>& lst = drivers;
        for (unsigned int i=0; i<lst.size(); i++) {
            printf("*** Removing %s\n",names[i].c_str());
            Drivers::factory().remove(names[i].c_str());
            //printf("*** removed %s\n",names[i].c_str());
            delete lst[i];
            //printf("*** deleted %s\n",names[i].c_str());
        }
        lst.clear();
        names.clear();
        mutex.post();
    }

    void update() {
        mutex.wait();
        PlatformVector<PolyDriver *>& lst = drivers;
        for (unsigned int i=0; i<lst.size(); i++) {
            if (needDrive[i]) {
                IService *service;
                lst[i]->view(service);
                if (service!=NULL) {
                    service->updateService();
                }
            }
        }
        mutex.post();
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

    bool add(const ConstString& name, yarp::os::Searchable& config) {
        //printf("ADDING %s\n", config.toString().c_str());
        PolyDriver *pd = new PolyDriver();
        yAssert(pd!=NULL);
        bool result = pd->open(config);
        if (!result) {
            delete pd;
            return false;
        }
        drivers.push_back(pd);
        names.push_back(name);
        IService *service = NULL;
        pd->view(service);
        bool backgrounded = true;
        if (service!=NULL) {
            backgrounded = service->startService();
            if (backgrounded) {
                // we don't need to poll this, so forget about the
                // service interface
                printf("group: service backgrounded\n");
                service = NULL;
            }
        }
        needDrive.push_back(!backgrounded);
        needDriveSummary = needDriveSummary || (!backgrounded);
        Drivers::factory().add(new DriverLinkCreator(name,*pd));
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
    return false;
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

bool DeviceGroup::startService() {
    return !HELPER(implementation).needDriveSummary;
}


bool DeviceGroup::updateService() {
    HELPER(implementation).update();
    return true;
}


DeviceGroup::~DeviceGroup() {
    if (implementation!=NULL) {
        delete &HELPER(implementation);
        implementation = NULL;
    }
}


