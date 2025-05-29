/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_SERIALPORT_NWC_H
#define YARP_DEV_SERIALPORT_NWC_H

#include <cstdio>
#include <cstdlib>

#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ISerialDevice.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include "ISerialMsgs.h"

#include "SerialPort_nwc_yarp_ParamsParser.h"

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

/**
 * @ingroup dev_impl_nwc_yarp
 *
 * \brief `serialPort_nwc_yarp`: a client for a serialPort_nws_yarp device
 *
 * Parameters required by this device are shown in class: SerialPort_nwc_yarp_ParamsParser
 */
class SerialPort_nwc_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::ISerialDevice,
        public SerialPort_nwc_yarp_ParamsParser
{
private:
    ISerialMsgs               m_rpc;
    yarp::os::Port            m_rpcPort;

    bool closeMain()
    {
        m_rpcPort.close();
        return true;
    }

public:
    SerialPort_nwc_yarp() = default;
    SerialPort_nwc_yarp(const SerialPort_nwc_yarp&) = delete;
    SerialPort_nwc_yarp(SerialPort_nwc_yarp&&) = delete;
    SerialPort_nwc_yarp& operator=(const SerialPort_nwc_yarp&) = delete;
    SerialPort_nwc_yarp& operator=(SerialPort_nwc_yarp&&) = delete;
    ~SerialPort_nwc_yarp() override;

    // ISerialDevice methods
    yarp::dev::ReturnValue sendString(const std::string& msg) override;
    yarp::dev::ReturnValue sendBytes(const std::vector<unsigned char>& line) override;
    yarp::dev::ReturnValue sendByte(unsigned char byt) override;
    yarp::dev::ReturnValue receiveString(std::string& msg) override;
    yarp::dev::ReturnValue receiveBytes(std::vector<unsigned char>& line, const int MaxSize) override;
    yarp::dev::ReturnValue receiveByte(unsigned char& chr) override;
    yarp::dev::ReturnValue receiveLine(std::vector<char>& line, const int MaxLineLength) override;
    yarp::dev::ReturnValue setDTR(bool enable) override;
    yarp::dev::ReturnValue flush(size_t& flushed_chars) override;
    yarp::dev::ReturnValue flush() override;

    bool close() override;
    bool open(Searchable& prop) override;

};

#endif // YARP_DEV_SERVERSERIAL_H
