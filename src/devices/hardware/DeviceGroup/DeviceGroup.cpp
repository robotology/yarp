/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "DeviceGroup.h"

#include <cstdio>
#include <yarp/os/Time.h>
#include <yarp/os/LogComponent.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/DriverLinkCreator.h>

#include <yarp/dev/AudioVisualInterfaces.h>

#include <mutex>
#include <vector>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(DEVICEGROUP, "yarp.devices.DeviceGroup")
}

#define HELPER(x) (*((DeviceGroupHelper*)(x)))


class DeviceGroupHelper
{
private:
    std::vector<PolyDriver *> drivers;
    std::vector<std::string> names;
    std::vector<bool> needDrive;
    std::mutex mutex;
public:
    bool needDriveSummary{false};

    void clear()
    {
        mutex.lock();
        std::vector<PolyDriver *>& lst = drivers;
        for (unsigned int i=0; i<lst.size(); i++) {
            yCInfo(DEVICEGROUP, "*** Removing %s",names[i].c_str());
            Drivers::factory().remove(names[i].c_str());
            yCTrace(DEVICEGROUP, "*** removed %s",names[i].c_str());
            delete lst[i];
            yCTrace(DEVICEGROUP, "*** deleted %s",names[i].c_str());
        }
        lst.clear();
        names.clear();
        mutex.unlock();
    }

    void update()
    {
        mutex.lock();
        std::vector<PolyDriver *>& lst = drivers;
        for (unsigned int i=0; i<lst.size(); i++) {
            if (needDrive[i]) {
                IService *service;
                lst[i]->view(service);
                if (service!=nullptr) {
                    service->updateService();
                }
            }
        }
        mutex.unlock();
    }

    bool close()
    {
        yCTrace(DEVICEGROUP, "*** Device group closing");
        clear();
        yCTrace(DEVICEGROUP, "*** Device group closed");
        return true;
    }

    ~DeviceGroupHelper()
    {
        clear();
    }

    bool add(const std::string& name, yarp::os::Searchable& config)
    {
        yCTrace(DEVICEGROUP, "ADDING %s", config.toString().c_str());
        auto* pd = new PolyDriver();
        yCAssert(DEVICEGROUP, pd!=nullptr);
        bool result = pd->open(config);
        if (!result) {
            delete pd;
            return false;
        }
        drivers.push_back(pd);
        names.push_back(name);
        IService *service = nullptr;
        pd->view(service);
        bool backgrounded = true;
        if (service!=nullptr) {
            backgrounded = service->startService();
            if (backgrounded) {
                // we don't need to poll this, so forget about the
                // service interface
                yCInfo(DEVICEGROUP, "group: service backgrounded");
                service = nullptr;
            }
        }
        needDrive.push_back(!backgrounded);
        needDriveSummary = needDriveSummary || (!backgrounded);
        Drivers::factory().add(new DriverLinkCreator(name,*pd));
        return true;
    }

};




bool DeviceGroup::open(yarp::os::Searchable& config)
{
    if (implementation==nullptr) {
        implementation = new DeviceGroupHelper;
    }
    if (implementation==nullptr) {
        yCError(DEVICEGROUP, "Out of memory");
        return false;
    }

    if (config.check("part","a list of section names, with each section containing a device")) {
        Bottle bot = config.findGroup("part").tail();
        yCInfo(DEVICEGROUP, "Assembly of: %s", bot.toString().c_str());
        for (size_t i=0; i<bot.size(); i++) {
            std::string name = bot.get(i).asString();
            yCInfo(DEVICEGROUP, "  %s -> %s", name.c_str(),
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
                      yarp::os::Searchable& config, const char *comment)
{
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
            yCError(DEVICEGROUP, "cannot make <%s>", name->toString().c_str());
            return false;
        }
    } else {
        yCError(DEVICEGROUP, "\"--%s <name>\" not set", key);
        return false;
    }
    return true;
}


bool DeviceGroup::closeMain()
{
    yCInfo(DEVICEGROUP, "Devices closing");
    HELPER(implementation).close();
    source.close();
    sink.close();
    return true;
}

bool DeviceGroup::startService()
{
    return !HELPER(implementation).needDriveSummary;
}


bool DeviceGroup::updateService()
{
    HELPER(implementation).update();
    return true;
}


DeviceGroup::~DeviceGroup()
{
    if (implementation!=nullptr) {
        delete &HELPER(implementation);
        implementation = nullptr;
    }
}
