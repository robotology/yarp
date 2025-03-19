/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include <yarp/os/RpcServer.h>
#include <yarp/dev/testInterfaces/IFakeDeviceInterfaceTest1.h>

#include "fakeDeviceServerImpl.h"

/**
* @ingroup dev_impl_nws_yarp
*
* \brief `FakeDevice_nws_yarp`: NWS device for testing purposes
*
*/
class FakeDevice_nws_yarp : public yarp::dev::DeviceDriver,
                            public yarp::dev::WrapperSingle,
                            public yarp::os::PortReader
{
protected:
    FakeServerRPCd      m_RPC;
    yarp::os::RpcServer m_RpcPort;
    yarp::dev::test::IFakeDeviceInterfaceTest1     *m_iFake = nullptr;
    const std::string   m_rpc_port_name = "/fake_nws/rpc";

public:
    // From DeviceDriver
    virtual bool open(yarp::os::Searchable &prop) override;
    virtual bool close() override;

    // From WrapperSingle
    virtual bool attach(yarp::dev::PolyDriver *drv) override;
    virtual bool detach() override;

    // From PortReader
    bool read(yarp::os::ConnectionReader& connection) override;
};
