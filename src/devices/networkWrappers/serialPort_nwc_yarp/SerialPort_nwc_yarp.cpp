/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "SerialPort_nwc_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Os.h>

namespace {
YARP_LOG_COMPONENT(SERIAL_NWC, "yarp.devices.serialPort_nwc_yarp")
}

SerialPort_nwc_yarp::~SerialPort_nwc_yarp()
{
    closeMain();
}

ReturnValue SerialPort_nwc_yarp::sendString(const std::string& msg)
{
    auto ret = m_rpc.sendString(msg);
    return ret;
}

ReturnValue SerialPort_nwc_yarp::sendBytes(const std::vector<unsigned char>& line)
{
    auto ret = m_rpc.sendBytes(line);
    return ret;
}

ReturnValue SerialPort_nwc_yarp::sendByte(unsigned char chr)
{
    auto ret = m_rpc.sendByte(chr);
    return ret;
}

ReturnValue SerialPort_nwc_yarp::receiveString(std::string& msg)
{
    auto ret = m_rpc.receiveString();
    msg = ret.message;
    return ret.retval;
}

ReturnValue SerialPort_nwc_yarp::receiveByte(unsigned char& c)
{
    auto ret = m_rpc.receiveByte();
    c = ret.message;
    return ret.retval;
}

ReturnValue SerialPort_nwc_yarp::flush()
{
    ReturnValue ret = m_rpc.flush();
    return ret;
}

ReturnValue SerialPort_nwc_yarp::flush(size_t& flushed)
{
    auto ret = m_rpc.flushWithRet();
    flushed = ret.flushed_bytes;
    return ret.retval;

}
ReturnValue SerialPort_nwc_yarp::receiveLine(std::vector<char>& line, const int MaxLineLength)
{
    auto ret = m_rpc.receiveLine(MaxLineLength);
    line = ret.message;
    return ret.retval;
}

ReturnValue SerialPort_nwc_yarp::receiveBytes(std::vector<unsigned char>& line, const int MaxSize)
{
    auto ret = m_rpc.receiveBytes(MaxSize);
    line = ret.message;
    return ret.retval;
}

ReturnValue SerialPort_nwc_yarp::setDTR(bool enable)
{
    return m_rpc.setDTR(enable);
}

bool SerialPort_nwc_yarp::close()
{
    return closeMain();
}

bool SerialPort_nwc_yarp::open(Searchable& config)
{
    if (!parseParams(config)) { return false; }

    {
        std::string local_rpc = m_local;
        std::string remote_rpc = m_remote;

        if (!m_rpcPort.open(local_rpc))
        {
            yCError(SERIAL_NWC, "open() error could not open rpc port %s, check network", local_rpc.c_str());
            return false;
        }

        if (!Network::connect(local_rpc, remote_rpc))
        {
            yCError(SERIAL_NWC, "open() error could not connect to %s", remote_rpc.c_str());
            return false;
        }

        if (!m_rpc.yarp().attachAsClient(m_rpcPort))
        {
            yCError(SERIAL_NWC, "Error! Cannot attach the m_rpc_port port as a client");
            return false;
        }
    }

    //Check the protocol version
    if (!m_rpc.checkProtocolVersion()) { return false; }

    yCInfo(SERIAL_NWC) << "Opening of NWC successful";

    return true;
}
