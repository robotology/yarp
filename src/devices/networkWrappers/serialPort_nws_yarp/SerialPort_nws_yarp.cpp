/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "SerialPort_nws_yarp.h"

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

bool SerialPort_nws_yarp::open(Searchable& config)
{
    if (!parseParams(config)) { return false; }

    std::string rootName = m_name;

    if (!m_rpcPort.open(rootName+"/rpc"))
    {
        yCError(SERIAL_NWS, "Failed to open rpc port");
        return false;
    }
    m_rpcPort.setReader(*this);

    yCInfo(SERIAL_NWS, "Device waiting for attach...");
    return true;
}

void SerialPort_nws_yarp::run()
{
    yCInfo(SERIAL_NWS, "Server Serial starting");

    //double before, now;
    while (!isStopping())
    {
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

    m_rpc.setInterfaces(m_iserial);

    start();
    return true;
}

bool  SerialPort_nws_yarp::detach()
{
    m_iserial = nullptr;
    return true;
}

bool SerialPort_nws_yarp::closeMain()
{
    if (Thread::isRunning()) {
        Thread::stop();
    }
    //close the port connection here
    m_rpcPort.close();
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
ReturnValue ISerialMsgsd::setDTR(bool enable)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    if (!m_iser)
    {
        yCError(SERIAL_NWS, "ISerialDevice interface was not set");
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    return m_iser->setDTR(enable);
}

ReturnValue ISerialMsgsd::flush()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    if (!m_iser)
    {
        yCError(SERIAL_NWS, "ISerialDevice interface was not set");
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    return m_iser->flush();
}

return_flush ISerialMsgsd::flushWithRet()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_flush ret;
    if (!m_iser)
    {
        yCError(SERIAL_NWS, "ISerialDevice interface was not set");
        ret.retval = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    size_t val;
    ret.retval = m_iser->flush(val);
    ret.flushed_bytes = val;
    return ret;
}

ReturnValue ISerialMsgsd::sendString(const std::string& message)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    if (!m_iser)
    {
        yCError(SERIAL_NWS, "ISerialDevice interface was not set");
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    ReturnValue ret = m_iser->sendString(message);
    return ret;
}

ReturnValue ISerialMsgsd::sendBytes(const std::vector<std::int8_t>& message)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    if (!m_iser) {
        yCError(SERIAL_NWS, "ISerialDevice interface was not set");
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    ReturnValue ret = m_iser->sendBytes(message);
    return ret;
}

ReturnValue ISerialMsgsd::sendByte(const std::int8_t message)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    if (!m_iser) {
        yCError(SERIAL_NWS, "ISerialDevice interface was not set");
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    ReturnValue ret = m_iser->sendByte(message);
    return ret;
}

return_receiveString ISerialMsgsd::receiveString()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_receiveString ret;
    if (!m_iser) {
        yCError(SERIAL_NWS, "ISerialDevice interface was not set");
        ret.retval = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    std::string msg;
    ret.retval = m_iser->receiveString(msg);
    ret.message = msg;
    return ret;
}

return_receiveBytes ISerialMsgsd::receiveBytes(const std::int32_t maxNumberOfByes)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_receiveBytes ret;
    if (!m_iser) {
        yCError(SERIAL_NWS, "ISerialDevice interface was not set");
        ret.retval = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    std::vector<unsigned char> msg;
    ret.retval = m_iser->receiveBytes(msg, maxNumberOfByes);
    ret.message = msg;
    return ret;
}

return_receiveByte ISerialMsgsd::receiveByte()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_receiveByte ret;
    if (!m_iser) {
        yCError(SERIAL_NWS, "ISerialDevice interface was not set");
        ret.retval = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    unsigned char msg;
    ret.retval = m_iser->receiveByte (msg);
    ret.message = msg;
    return ret;
}

return_receiveLine ISerialMsgsd::receiveLine(const std::int32_t maxNumberOfByes)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_receiveLine ret;
    if (!m_iser) {
        yCError(SERIAL_NWS, "ISerialDevice interface was not set");
        ret.retval = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
}
