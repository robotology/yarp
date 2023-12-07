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

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

/**
 * @ingroup dev_impl_nwc_yarp
 *
 * \brief `serialPort_nwc_yarp`: a client for a serialPort_nws_yarp device
 *
 */
class SerialPort_nwc_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::ISerialDevice
{
private:
    ISerialMsgs               m_rpc;
    yarp::os::Port            m_rpcPort;
    yarp::os::Port            m_sendPort;
    yarp::os::Port            m_receivePort;

    bool closeMain()
    {
        m_rpcPort.close();
        m_sendPort.close();
        m_receivePort.close();
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
    bool send(const Bottle& msg) override;
    bool send(const char *msg, size_t size) override;
    bool receive(Bottle& msg) override;
    int receiveChar(char& c) override;
    int flush () override;
    int receiveLine(char* line, const int MaxLineLength) override;
    int receiveBytes(unsigned char* bytes, const int size) override;
    bool setDTR(bool enable) override;
    bool close() override;
    bool open(Searchable& prop) override;

};

#endif // YARP_DEV_SERVERSERIAL_H
