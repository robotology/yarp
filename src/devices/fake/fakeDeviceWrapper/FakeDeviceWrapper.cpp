/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeDeviceWrapper.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/ControlBoardInterfaces.h>

using namespace yarp::os;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(FAKEDEVICEWRAPPER, "yarp.device.fakeDeviceWrapper")
}

yarp::dev::ReturnValue FakeDeviceWrapper::testSetValue(int value)
{
    if (!iTest) { yCError(FAKEDEVICEWRAPPER) << "Invalid/not yet attached interface"; }

    yCInfo(FAKEDEVICEWRAPPER) << "testSetValue() called";
    return iTest->testSetValue(value);
}

yarp::dev::ReturnValue FakeDeviceWrapper::testGetValue(int& value)
{
    if (!iTest) { yCError(FAKEDEVICEWRAPPER) << "Invalid/not yet attached interface"; }

    yCInfo(FAKEDEVICEWRAPPER) << "testGetValue() called";
    return iTest->testGetValue(value);
}

bool FakeDeviceWrapper::open(yarp::os::Searchable& config)
{
    this->m_RpcPort.open("/FakeDeviceWrapper/rpc");
    m_RpcPort.setReader(*this);

    yCInfo(FAKEDEVICEWRAPPER) << "FakeDeviceWrapper opened";
    return true;
}

bool FakeDeviceWrapper::close()
{
    yCInfo(FAKEDEVICEWRAPPER) << "FakeDeviceWrapper closed";
    return true;
}

bool FakeDeviceWrapper::detach()
{
    iTest=nullptr;
    return true;
}

bool FakeDeviceWrapper::attach(yarp::dev::PolyDriver* drv)
{
    if (drv->isValid())
    {
        drv->view(iTest);
        return true;
    }
    return false;
}

bool FakeDeviceWrapper::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    ReturnValue retval;

    bool ok = command.read(connection);
    if (!ok) {
        return false;
    }
    reply.clear();

    if (command.get(0).asString() == "testSetValue")
    {
        retval = testSetValue(1);
        if (retval) {reply.addVocab32(VOCAB_OK);}
        else  {reply.addVocab32(VOCAB_ERR);}
    }
    else
    if (command.get(0).asString() == "testGetValue")
    {
        int val=0;
        retval = testGetValue(val);
        if (retval) {
            reply.addVocab32(VOCAB_OK);
        }
        else {
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else
    {
        reply.addVocab32(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        reply.write(*returnToSender);
    }
    return true;
}
