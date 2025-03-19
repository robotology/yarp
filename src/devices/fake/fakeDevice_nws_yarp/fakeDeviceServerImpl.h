/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/dev/testInterfaces/IFakeDeviceInterfaceTest1.h>

#include "FakeTestMsgs.h"

class FakeServerRPCd : public FakeTestMsgs
{
private:
    yarp::dev::test::IFakeDeviceInterfaceTest1* m_iFake = nullptr;

public:

    void setInterface(yarp::dev::test::IFakeDeviceInterfaceTest1* _iFake);
    void unsetInterface();

    // From FakeTestMsgs
    yarp::dev::ReturnValue doSomethingRPC() override;
};
