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
    int count = 1;
public:
    Property info;

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
    m_dd(nullptr),
    m_Priv(nullptr)
{
}

PolyDriver::PolyDriver(const std::string& txt) :
    DeviceDriver(),
    m_dd(nullptr),
    m_Priv(nullptr)
{
    open(txt);
}

PolyDriver::PolyDriver(yarp::os::Searchable& config) :
    DeviceDriver(),
    m_dd(nullptr),
    m_Priv(nullptr)
{
    open(config);
}

PolyDriver::~PolyDriver()
{
    close();
    yCAssert(POLYDRIVER, m_dd == nullptr);
    yCAssert(POLYDRIVER, m_Priv == nullptr);
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
    if (m_Priv==nullptr) {
        m_Priv = new PolyDriver::Private;
    }
    yCAssert(POLYDRIVER, m_Priv != nullptr);

    coreOpen(config);
    m_Priv->info.fromString(config.toString());

    return isValid();
}


bool PolyDriver::close()
{
    bool result = false;
    if (m_Priv !=nullptr) {
        int ct = m_Priv->removeRef();
        if (ct==0) {
            yCAssert(POLYDRIVER, m_Priv != nullptr);
            delete m_Priv;
            m_Priv = nullptr;
            if (m_dd !=nullptr) {
                result = m_dd->close();
                delete m_dd;
                m_dd = nullptr;
            } else {
                result = true;
            }
        }
        m_dd = nullptr;
        m_Priv = nullptr;
    }
    return result;
}

bool PolyDriver::isValid() const
{
    return m_dd != nullptr;
}

bool PolyDriver::link(PolyDriver& alt)
{
    if (!alt.isValid()) {
        return false;
    }
    if (isValid()) {
        return false;
    }
    m_dd = alt.m_dd;
    if (m_Priv !=nullptr) {
        int ct = m_Priv->removeRef();
        if (ct==0) {
            yCAssert(POLYDRIVER, m_Priv != nullptr);
            delete m_Priv;
        }
    }
    m_Priv = alt.m_Priv;
    yCAssert(POLYDRIVER, m_dd != nullptr);
    yCAssert(POLYDRIVER, m_Priv != nullptr);
    m_Priv->addRef();
    return true;
}

bool PolyDriver::coreOpen(yarp::os::Searchable& prop)
{
    setId(prop.check("id", prop.check("device", Value("")), "Id assigned to this device").toString());
    yarp::os::Searchable *config = &prop;
    Property p;
    std::string device_name = prop.toString();
    Value *part;
    if (prop.check("device",part)) {
        device_name = part->toString();
    }

    DeviceDriver *driver = nullptr;

    DriverCreator *deviceCreator = Drivers::factory().find(device_name.c_str());
    if (deviceCreator !=nullptr)
    {
        Value *val;
        //if the device has a wrapper..
        if (config->check("wrapping_enabled",val) && (!deviceCreator->getWrapper().empty()))
        {
            std::string wrapper_name = deviceCreator->getWrapper();
            DriverCreator *wrapperCreator = Drivers::factory().find(wrapper_name.c_str());
            // and this wrapper exists..
            if (wrapperCreator !=nullptr)
            {
                p.fromString(config->toString());
                p.unput("wrapping_enabled");
                config = &p;
                //and this wrapper is not the device itself..
                if (wrapperCreator != deviceCreator)
                {
                    //..than open the wrapper instead of the device.
                    //this operation is done using the deviceBundler plugin, and passing to it
                    //the name of devices that it has to open and attach.
                    p.put("attached_device", device_name);
                    p.put("wrapper_device", wrapper_name);
                    p.put("device", "deviceBundler");
                    DriverCreator* bundlerCreator = Drivers::factory().find("deviceBundler");
                    driver = bundlerCreator->create();
                    deviceCreator = bundlerCreator;
                }
                else
                {
                    //otherwise the device itself is already the wrapper
                    driver = deviceCreator->create();
                }
            }
        }
        //..the device does not have a wrapper
        else
        {
            driver = deviceCreator->create();
        }
    }
    else
    {
        // FIXME do not use yarpdev here
        yCIError(POLYDRIVER, id(), "Could not find device <%s>", device_name.c_str());
        return false;
    }

    if (driver!=nullptr)
    {
        PolyDriver *manager = deviceCreator->owner();
        if (manager!=nullptr)
        {
            link(*manager);
            return true;
        }

        std::string param_string = config->toString();
        yCIDebug(POLYDRIVER, id(), "Parameters are %s", param_string.c_str());
        driver->setId(id());
        //try to open the device:
        bool ok = driver->open(*config);
        //if the device did not open successfully
        if (!ok)
        {
            yCIError(POLYDRIVER, id(), "Driver <%s> was found but could not open", config->find("device").toString().c_str());
            delete driver;
            driver = nullptr;
        }
        //if the device opened successfully
        else
        {
            //if the device is deprecated...
            yarp::dev::DeprecatedDeviceDriver *ddd = nullptr;
            driver->view(ddd);
            if(ddd)
            {
                //but the user requested it explicitly, than just print a warning
                if(config->check("allow-deprecated-devices")) {
                    yCIWarning(POLYDRIVER, id(), R"(Device "%s" is deprecated. Opening since the "allow-deprecated-devices" option was passed in the configuration.)", device_name.c_str());
                //if it is not requested explicitly, then close it with an error
                } else {
                    yCIError(POLYDRIVER, id(), R"(Device "%s" is deprecated. Pass the "allow-deprecated-devices" option in the configuration if you want to open it anyway.)", device_name.c_str());
                    driver->close();
                    delete driver;
                    return false;
                }
            }
            //print some info
            std::string name = deviceCreator->getName();
            std::string wrapper = deviceCreator->getWrapper();
            std::string code = deviceCreator->getCode();
            yCIInfo(POLYDRIVER, id(), "Created %s <%s>. See C++ class %s for documentation.",
                  ((name==wrapper)?"wrapper":"device"),
                  name.c_str(),
                  code.c_str());
        }
        m_dd = driver;
        return true;
    }

    return false;
}


DeviceDriver *PolyDriver::take()
{
    // this is not very careful
    DeviceDriver *result = m_dd;
    m_dd = nullptr;
    return result;
}

bool PolyDriver::give(DeviceDriver *dd, bool own)
{
    close();
    this->m_dd = dd;
    if (dd!=nullptr) {
        if (m_Priv ==nullptr) {
            m_Priv = new PolyDriver::Private;
        }
        yCAssert(POLYDRIVER, m_Priv != nullptr);
        if (!own) {
            m_Priv->addRef();
        }
    }
    return true;
}

DeviceDriver* PolyDriver::getImplementation()
{
    if(isValid()) {
        return m_dd->getImplementation();
    } else {
        return nullptr;
    }
}
