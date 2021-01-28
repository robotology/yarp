/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_DEVICEPIPE_H
#define YARP_DEV_DEVICEPIPE_H

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ServiceInterfaces.h>


/**
 * @ingroup dev_impl_other
 *
 * \brief `pipe`: Tries to connect the output of one device to the input of another.
 */
class DevicePipe :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IService
{
public:
    DevicePipe() = default;
    DevicePipe(const DevicePipe&) = delete;
    DevicePipe(DevicePipe&&) = delete;
    DevicePipe& operator=(const DevicePipe&) = delete;
    DevicePipe& operator=(DevicePipe&&) = delete;
    ~DevicePipe() override = default;

    bool open(yarp::os::Searchable& config) override;

    bool close() override;

    bool startService() override
    {
        // please call updateService
        return false;
    }

    bool stopService() override
    {
        return close();
    }

    bool updateService() override;

protected:
    yarp::dev::PolyDriver source;
    yarp::dev::PolyDriver sink;

    static bool open(const char* key,
                     yarp::dev::PolyDriver& poly,
                     yarp::os::Searchable& config,
                     const char* comment);
};


#endif // YARP_DEV_DEVICEPIPE_H
