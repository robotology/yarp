/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ROBOTDESCRIPTIONSERVER_H
#define YARP_DEV_ROBOTDESCRIPTIONSERVER_H

#include <mutex>
#include <string>

#include <yarp/dev/ReturnValue.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/IRobotDescription.h>

#include "IRobotDescriptionMsgs.h"

#include "RobotDescriptionStorage.h"
#include "RobotDescriptionServer_ParamsParser.h"

class IRobotDescriptiond : public IRobotDescriptionMsgs
{
    private:
    yarp::dev::IRobotDescription*   m_storage = nullptr;
    std::mutex                      m_mutex;

    public:
    IRobotDescriptiond () { m_storage = new RobotDescriptionStorage; }
    virtual ~IRobotDescriptiond()  { delete m_storage; }

    virtual return_getAllDevices getAllDevicesRPC()  override;
    virtual return_getAllDevicesByType getAllDevicesByTypeRPC(const std::string& type) override;
    virtual yarp::dev::ReturnValue registerDeviceRPC(const yarp::dev::DeviceDescription& dev) override;
    virtual yarp::dev::ReturnValue unregisterDeviceRPC(const std::string& dev) override;
    virtual yarp::dev::ReturnValue unregisterAllRPC() override;

    std::mutex* getMutex() {return &m_mutex;}
};

/**
* @ingroup dev_impl_wrapper
*
* \brief `robotDescriptionServer`: This device is a storage which contains a list of the currently opened device drivers.
*
* yarprobotinterfaces adds/removes devices to the storage using attachAll()/detachAll() methods.
* A robotDescriptionClient devices can used by the user to retrieve information about the currently opened devices.
*
* Parameters required by this device are shown in class: RobotDescriptionServer_ParamsParser
*/

class RobotDescriptionServer :
        public yarp::dev::DeviceDriver,
        public yarp::os::PortReader,
        public yarp::dev::IMultipleWrapper,
        public RobotDescriptionServer_ParamsParser
{
protected:
    std::mutex                                m_mutex;
    yarp::os::Port                            m_rpc_port;

    //thrift
    std::unique_ptr <IRobotDescriptiond>      m_RPC;

public:
    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    bool detachAll() override;
    bool attachAll(const yarp::dev::PolyDriverList &l) override;
    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_DEV_ROBOTDESCRIPTIONSERVER_H
