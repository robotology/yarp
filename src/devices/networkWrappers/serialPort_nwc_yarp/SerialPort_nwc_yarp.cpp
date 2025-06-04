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

bool SerialPort_nwc_yarp::send(const Bottle& msg)
{
    m_sendPort.write(msg);
    return true;
}

bool SerialPort_nwc_yarp::send(const char *msg, size_t size)
{
    Bottle b;
    b.addString(std::string(msg));
    m_sendPort.write(b);
    return true;
}

bool SerialPort_nwc_yarp::receive(Bottle& msg)
{
    return false;
}

int SerialPort_nwc_yarp::receiveChar(char& c)
{
    return 0;
}

int SerialPort_nwc_yarp::flush()
{
    int ret = m_rpc.flush();
    return ret;
}

int SerialPort_nwc_yarp::receiveLine(char* line, const int MaxLineLength)
{
    return 0;
}

int SerialPort_nwc_yarp::receiveBytes(unsigned char* bytes, const int size)
{
    return 0;
}

bool SerialPort_nwc_yarp::setDTR(bool enable)
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

    {
        std::string local_send = "/serialPort_nwc_yarp/out";
        std::string remote_send = "/serialPort_nws_yarp/in";
        if (!m_sendPort.open(local_send))
        {
            yCError(SERIAL_NWC, "open() error could not open rpc port %s, check network", local_send.c_str());
            return false;
        }

        if (!Network::connect(local_send, remote_send))
        {
            yCError(SERIAL_NWC, "open() error could not connect to %s", remote_send.c_str());
            return false;
        }
    }

    //Check the protocol version
    if (!m_rpc.checkProtocolVersion()) { return false; }

    yCInfo(SERIAL_NWC) << "Opening of NWC successful";
    return true;
}
