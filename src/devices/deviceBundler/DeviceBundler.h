/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEVICEBUNDLER_H
#define YARP_DEVICEBUNDLER_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IWrapper.h>
#include <yarp/dev/ServiceInterfaces.h>
#include "DeviceBundler_ParamsParser.h"

/**
* @ingroup dev_impl_wrapper
*
* \brief `DeviceBundler`: A device capable of opening two additional devices, specified by the user, and
* perform an attach operation.
*
* This device is typically used by yarpdev executable to open a wrapper and subdevice using a single command line.
* For more complex operations, such as opening and attaching more than two devices, it is recommended to
* use yarprobotinterface instead.
*
* Parameters required by this device are shown in class: DeviceBundler_ParamsParser
*/

class DeviceBundler :  public yarp::dev::DeviceDriver,
                       public yarp::dev::IService,
                       public DeviceBundler_ParamsParser
{
public:
    DeviceBundler();
    DeviceBundler(const DeviceBundler&) = delete;
    DeviceBundler(DeviceBundler&&) = delete;
    DeviceBundler& operator=(const DeviceBundler&) = delete;
    DeviceBundler& operator=(DeviceBundler&&) = delete;

    ~DeviceBundler() override;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //  yarp::dev::IService methods
    virtual bool startService() override;
    virtual bool updateService() override;
    virtual bool stopService() override;

private:
    yarp::dev::PolyDriver m_pdev_wrapper;
    yarp::dev::PolyDriver m_pdev_subdevice;
    yarp::dev::IWrapper*  m_iWrapper=nullptr;
    yarp::dev::IService* m_iService=nullptr;
};

#endif // YARP_DEVICEBUNDLER_H
