/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "serialPort_nwc_yarp.h"

#include <yarp/os/LogComponent.h>
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
    return false;
}

bool SerialPort_nwc_yarp::send(char *msg, size_t size)
{
    return false;
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

bool SerialPort_nwc_yarp::open(Searchable& prop)
{
    std::string local_rpc =
        prop.check("local",Value("/serialPort_nwc_yarp"),
                    "local rpc port name").asString();
    std::string remote_rpc =
        prop.check("remote", Value("/serialPort_nws_yarp/rpc"),
                   "remote rpc port name").asString();

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

    return true;
}
