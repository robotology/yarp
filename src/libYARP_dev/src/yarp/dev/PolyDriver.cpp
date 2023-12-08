/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/PolyDriver.h>

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/Property.h>

using namespace yarp::os;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(POLYDRIVER, "yarp.dev.PolyDriver")
}

class PolyDriver::Private
{
private:
    Property comment;
    Property fallback;
    Property present;
    Property actual;
    Bottle order;
    int count = 1;

public:
    Property info;

    Bottle getOptions()
    {
        return order;
    }

    std::string getComment(const char *option)
    {
        std::string desc = comment.find(option).toString();
        return desc;
    }

    Value getDefaultValue(const char *option)
    {
        return fallback.find(option);
    }

    Value getValue(const char *option)
    {
        return actual.find(option);
    }

    void addRef()
    {
        count++;
    }

    int removeRef()
    {
        count--;
        return count;
    }

    int getRef()
    {
        return count;
    }
};


PolyDriver::PolyDriver() :
    DeviceDriver(),
    dd(nullptr),
    mPriv(nullptr)
{
}

PolyDriver::PolyDriver(const std::string& txt) :
    DeviceDriver(),
    dd(nullptr),
    mPriv(nullptr)
{
    open(txt);
}

PolyDriver::PolyDriver(yarp::os::Searchable& config) :
    DeviceDriver(),
    dd(nullptr),
    mPriv(nullptr)
{
    open(config);
}

PolyDriver::~PolyDriver()
{
    close();
    yCAssert(POLYDRIVER, dd == nullptr);
    yCAssert(POLYDRIVER, mPriv == nullptr);
}



bool PolyDriver::open(const std::string& txt)
{
    Property p;
    p.put("device",txt);
    return open(p);
}


bool PolyDriver::open(yarp::os::Searchable& config)
{
    if (isValid()) {
        // already open - should close first
        return false;
    }
    if (mPriv==nullptr) {
        mPriv = new PolyDriver::Private;
    }
    yCAssert(POLYDRIVER, mPriv != nullptr);

    coreOpen(config);
    mPriv->info.fromString(config.toString());

    return isValid();
}


bool PolyDriver::close()
{
    bool result = false;
    if (mPriv!=nullptr) {
        int ct = mPriv->removeRef();
        if (ct==0) {
            yCAssert(POLYDRIVER, mPriv != nullptr);
            delete mPriv;
            mPriv = nullptr;
            if (dd!=nullptr) {
                result = dd->close();
                delete dd;
                dd = nullptr;
            } else {
                result = true;
            }
        }
        dd = nullptr;
        mPriv = nullptr;
    }
    return result;
}

bool PolyDriver::isValid() const
{
    return dd != nullptr;
}

bool PolyDriver::link(PolyDriver& alt)
{
    if (!alt.isValid()) {
        return false;
    }
    if (isValid()) {
        return false;
    }
    dd = alt.dd;
    if (mPriv!=nullptr) {
        int ct = mPriv->removeRef();
        if (ct==0) {
            yCAssert(POLYDRIVER, mPriv != nullptr);
            delete mPriv;
        }
    }
    mPriv = alt.mPriv;
    yCAssert(POLYDRIVER, dd != nullptr);
    yCAssert(POLYDRIVER, mPriv != nullptr);
    mPriv->addRef();
    return true;
}

bool PolyDriver::coreOpen(yarp::os::Searchable& prop)
{
    setId(prop.check("id", prop.check("device", Value("")), "Id assigned to this device").toString());
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
        if (config->check("wrapped",val) && (!creator->getWrapper().empty())) {
            std::string wrapper = creator->getWrapper();
            DriverCreator *wrapCreator =
                Drivers::factory().find(wrapper.c_str());
            if (wrapCreator!=nullptr) {
                p.fromString(config->toString());
                p.unput("wrapped");
                config = &p;
                if (wrapCreator!=creator) {
                    p.put("subdevice", str);
                    p.put("device", wrapper);
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
        yCIError(POLYDRIVER, id(), "Could not find device <%s>", str.c_str());
        return false;
    }

    if (driver!=nullptr) {
        PolyDriver *manager = creator->owner();
        if (manager!=nullptr) {
            link(*manager);
            return true;
        }

        yCIDebug(POLYDRIVER, id(), "Parameters are %s", config->toString().c_str());
        driver->setId(id());
        bool ok = driver->open(*config);
        if (!ok) {
            yCIError(POLYDRIVER, id(), "Driver <%s> was found but could not open", config->find("device").toString().c_str());
            delete driver;
            driver = nullptr;
        } else {
            yarp::dev::DeprecatedDeviceDriver *ddd = nullptr;
            driver->view(ddd);
            if(ddd) {
                if(config->check("allow-deprecated-devices")) {
                    yCIWarning(POLYDRIVER, id(), R"(Device "%s" is deprecated. Opening since the "allow-deprecated-devices" option was passed in the configuration.)", str.c_str());
                } else {
                    yCIError(POLYDRIVER, id(), R"(Device "%s" is deprecated. Pass the "allow-deprecated-devices" option in the configuration if you want to open it anyway.)", str.c_str());
                    driver->close();
                    delete driver;
                    return false;
                }
            }
            std::string name = creator->getName();
            std::string wrapper = creator->getWrapper();
            std::string code = creator->getCode();
            yCIInfo(POLYDRIVER, id(), "Created %s <%s>. See C++ class %s for documentation.",
                  ((name==wrapper)?"wrapper":"device"),
                  name.c_str(),
                  code.c_str());
        }
        dd = driver;
        return true;
    }

    return false;
}


DeviceDriver *PolyDriver::take()
{
    // this is not very careful
    DeviceDriver *result = dd;
    dd = nullptr;
    return result;
}

bool PolyDriver::give(DeviceDriver *dd, bool own)
{
    close();
    this->dd = dd;
    if (dd!=nullptr) {
        if (mPriv==nullptr) {
            mPriv = new PolyDriver::Private;
        }
        yCAssert(POLYDRIVER, mPriv != nullptr);
        if (!own) {
            mPriv->addRef();
        }
    }
    return true;
}

DeviceDriver* PolyDriver::getImplementation()
{
    if(isValid()) {
        return dd->getImplementation();
    } else {
        return nullptr;
    }
}
