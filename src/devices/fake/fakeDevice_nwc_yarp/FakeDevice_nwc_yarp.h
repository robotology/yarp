/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/DeviceDriver.h>

#include "FakeTestMsgs.h"
#include <yarp/dev/testInterfaces/IFakeDeviceInterfaceTest1.h>

 /**
 * @ingroup dev_impl_nwc_yarp
 *
 * \brief `FakeDevice_nwc_yarp`: NWC device for testing purposes
 *
 */
class FakeDevice_nwc_yarp : public yarp::dev::DeviceDriver,
                            public yarp::dev::test::IFakeDeviceInterfaceTest1
{
protected:
    // thrift interface
    FakeTestMsgs   m_thrift_RPC;

    // rpc port
    yarp::os::Port m_rpc_port_to_server;
    const std::string m_local = "/fake_nwc/rpc";
    const std::string m_remote = "/fake_nws/rpc";

public:

    //From DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //From IFakeInterface
    yarp::dev::ReturnValue doSomething() override;
    yarp::dev::ReturnValue doSomethingUnimplemented() override;
};
