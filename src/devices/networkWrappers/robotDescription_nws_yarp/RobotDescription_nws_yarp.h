/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ROBOTDESCRIPTION_NWS_YARP_H
#define YARP_DEV_ROBOTDESCRIPTION_NWS_YARP_H

#include <mutex>
#include <string>

#include <yarp/dev/ReturnValue.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/IRobotDescription.h>

#include "IRobotDescriptionMsgs.h"

#include "RobotDescription_nws_yarp_ParamsParser.h"

class IRobotDescriptiond : public IRobotDescriptionMsgs
{
    private:
    yarp::dev::IRobotDescription*   m_istorage = nullptr;
    std::mutex                      m_mutex;

    public:
    IRobotDescriptiond (yarp::dev::IRobotDescription* stor) { m_istorage = stor;}
    virtual ~IRobotDescriptiond()  { m_istorage=nullptr; }

    virtual return_getAllDevices getAllDevicesRPC()  override;
    virtual return_getAllDevicesByType getAllDevicesByTypeRPC(const std::string& type) override;
    virtual yarp::dev::ReturnValue registerDeviceRPC(const yarp::dev::DeviceDescription& dev) override;
    virtual yarp::dev::ReturnValue unregisterDeviceRPC(const std::string& dev) override;
    virtual yarp::dev::ReturnValue unregisterAllRPC() override;

    std::mutex* getMutex() {return &m_mutex;}
};

/**
* @ingroup dev_impl_nws_yarp
*
* \brief `RobotDescription_nws_yarp`: This device is a storage which contains a list of the currently opened device drivers.
*
* yarprobotinterfaces adds/removes devices to the storage using attachAll()/detachAll() methods.
* A robotDescription_nwc_yarp device can used by the user to retrieve information about the currently opened devices.
*
* Parameters required by this device are shown in class: RobotDescription_nws_yarp_ParamsParser
*/

class RobotDescription_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::os::PortReader,
        public yarp::dev::WrapperSingle,
        public RobotDescription_nws_yarp_ParamsParser
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

    bool detach() override;
    bool attach(yarp::dev::PolyDriver* driver) override;
    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_DEV_ROBOTDESCRIPTION_NWS_YARP_H
