/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_SERIALPORT_NWS_YARP_H
#define YARP_DEV_SERIALPORT_NWS_YARP_H

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
#include <yarp/os/RpcServer.h>
#include <yarp/dev/WrapperSingle.h>
#include <memory>
#include "ISerialMsgs.h"

#include "SerialPort_nws_yarp_ParamsParser.h"

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

class SerialPort_nws_yarp;

class ISerialMsgsd : public ISerialMsgs
{
private:
    std::mutex   m_mutex;
    yarp::dev::ISerialDevice* m_iser = nullptr;

public:
    yarp::dev::ReturnValue  sendString(const std::string& message) override;
    yarp::dev::ReturnValue  sendBytes(const std::vector<std::int8_t>& message) override;
    yarp::dev::ReturnValue  sendByte(const std::int8_t message) override;
    return_receiveString    receiveString() override;
    return_receiveBytes     receiveBytes(const std::int32_t maxNumberOfByes) override;
    return_receiveByte      receiveByte() override;
    return_receiveLine      receiveLine(const std::int32_t maxNumberOfByes) override;
    yarp::dev::ReturnValue  setDTR(const bool enable) override;
    yarp::dev::ReturnValue  flush() override;
    return_flush            flushWithRet() override;

public:
    void setInterfaces(yarp::dev::ISerialDevice* iser) {m_iser = iser;}
    std::mutex* getMutex() { return &m_mutex; }
};

/**
 * @ingroup dev_impl_nws_yarp
 *
 * \brief `serialPort_nws_yarp`: Export a serial sensor.
 *
 * The network interface is composed by two ports.
 * The input port accepts bottles and sends their contents as
 * text to the serial port.
 * The output port streams out whatever information it gets in the
 * serial port as text bottles.
 *
 * Parameters required by this device are shown in class: SerialPort_nws_yarp_ParamsParser
 */
class SerialPort_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::os::Thread,
        public yarp::dev::WrapperSingle,
        public yarp::os::PortReader,
        public SerialPort_nws_yarp_ParamsParser
{
private:
    yarp::dev::ISerialDevice* m_iserial{ nullptr };
    yarp::os::Port            m_rpcPort;
    ISerialMsgsd              m_rpc;

    // yarp::dev::IWrapper
    bool  attach(yarp::dev::PolyDriver* deviceToAttach) override;
    bool  detach() override;

private:
    bool closeMain();

public:
    SerialPort_nws_yarp() = default;
    SerialPort_nws_yarp(const SerialPort_nws_yarp&) = delete;
    SerialPort_nws_yarp(SerialPort_nws_yarp&&) = delete;
    SerialPort_nws_yarp& operator=(const SerialPort_nws_yarp&) = delete;
    SerialPort_nws_yarp& operator=(SerialPort_nws_yarp&&) = delete;
    virtual ~SerialPort_nws_yarp() override;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool read(yarp::os::ConnectionReader& connection) override;
    void run() override;
};

#endif // YARP_DEV_SERIALPORT_NWS_YARP_H
