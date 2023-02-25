/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "serialPort_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/Os.h>

namespace {
YARP_LOG_COMPONENT(SERIAL_NWS, "yarp.devices.serialPort_nws_yarp")
}

SerialPort_nws_yarp::~SerialPort_nws_yarp()
{
    closeMain();
}

bool SerialPort_nws_yarp::close()
{
    return closeMain();
}

bool SerialPort_nws_yarp::open(Searchable& prop)
{
    std::string rootName =
        prop.check("name",Value("/serial"),
                    "prefix for port names").asString();

    command_buffer.attach(toDevice);
    reply_buffer.attach(fromDevice);

    toDevice.open(rootName+"/in");
    fromDevice.open(rootName+"/out");

    if (!m_rpcPort.open(rootName+"/rpc"))
    {
        yCError(SERIAL_NWS, "Failed to open rpc port");
        return false;
    }
    m_rpcPort.setReader(*this);

    return true;
}

void SerialPort_nws_yarp::run()
{
    yCInfo(SERIAL_NWS, "Server Serial starting");

    //double before, now;
    while (!isStopping())
    {
        //before = SystemClock::nowSystem();
        Bottle& b = reply_buffer.get();
        b.clear();
        receive( b );
        /*if(b.size() > 0)*/ /* this line was creating a memory leak !! */
        reply_buffer.write();
        //now = SystemClock::nowSystem();
        // give other threads the chance to run
        yarp::os::SystemClock::delaySystem(0.010);
    }
    yCInfo(SERIAL_NWS, "Server Serial stopping");
}

bool  SerialPort_nws_yarp::attach(yarp::dev::PolyDriver* deviceToAttach)
{
    if (deviceToAttach->isValid())
    {
        deviceToAttach->view(m_iserial);
    }

    if (nullptr == m_iserial)
    {
        yCError(SERIAL_NWS, "Subdevice passed to attach method is invalid");
        return false;
    }

    yCInfo(SERIAL_NWS, "Attach done");

    callback_impl = new ImplementCallbackHelper2(m_iserial);
    command_buffer.useCallback(*callback_impl);
    m_rpc.setInterfaces(m_iserial);

    start();
    return true;
}

bool  SerialPort_nws_yarp::detach()
{
    m_iserial = nullptr;
    return true;
}

bool SerialPort_nws_yarp::receive(Bottle& msg)
{
    if (m_iserial != nullptr)
    {
        m_iserial->receive(msg);
        return true;
    }
    else
    {
        return false;
    }
}

int SerialPort_nws_yarp::receiveChar(char& c)
{
    if (m_iserial != nullptr)
    {
        return m_iserial->receiveChar(c);
    }
    else
    {
        return -1;
    }
}

bool SerialPort_nws_yarp::closeMain()
{
    if (Thread::isRunning()) {
        Thread::stop();
    }
    //close the port connection here
    command_buffer.disableCallback();
    toDevice.close();
    fromDevice.close();
    m_rpcPort.close();
    if (callback_impl) {delete callback_impl; callback_impl=nullptr;}
    return true;
}

bool SerialPort_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    bool b = m_rpc.read(connection);
    if (b)
    {
        return true;
    }
    else
    {
        yCError(SERIAL_NWS, "read() Command failed");
        return false;
    }
}

//--------------------------------------------------
// RPC methods
bool ISerialMsgsd::setDTR(bool enable)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    m_iser->setDTR(enable);
    return true;
}

int ISerialMsgsd::flush()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_iser->flush();
}

//--------------------------------------------------
// ImplementCallbackHelper class.
ImplementCallbackHelper2::ImplementCallbackHelper2(yarp::dev::ISerialDevice*x)
{
    if (x ==nullptr)
    {
        yCError(SERIAL_NWS, "Could not get ISerialDevice interface");
        std::exit(1);
    }
    m_iser= x;
}

void ImplementCallbackHelper2::onRead(Bottle &b)
{
    if (m_iser)
    {
        bool ok = m_iser->send(b);
        if (!ok)
        {
            yCError(SERIAL_NWS, "Problems while trying to send data");
        }
    }
}
