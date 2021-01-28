/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_DEVICEGROUP_H
#define YARP_DEV_DEVICEGROUP_H

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ServiceInterfaces.h>

/**
 * @ingroup dev_impl_other
 *
 * \brief `group`: Lets you make a bunch of devices as a group.
 */
class DeviceGroup :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IService
{
public:
    DeviceGroup() = default;
    DeviceGroup(const DeviceGroup&) = delete;
    DeviceGroup(DeviceGroup&&) = delete;
    DeviceGroup& operator=(const DeviceGroup&) = delete;
    DeviceGroup& operator=(DeviceGroup&&) = delete;
    ~DeviceGroup() override;

    bool open(yarp::os::Searchable& config) override;

    bool close() override
    {
        return closeMain();
    }

    bool startService() override;

    bool stopService() override
    {
        return close();
    }

    bool updateService() override;

private:
    void* implementation{nullptr};

    bool closeMain();

    yarp::dev::PolyDriver source;
    yarp::dev::PolyDriver sink;

    bool open(const char *key,
              yarp::dev::PolyDriver& poly,
              yarp::os::Searchable& config,
              const char *comment);
};


#endif // YARP_DEV_DEVICEGROUP_H
