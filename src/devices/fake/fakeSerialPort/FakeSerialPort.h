/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FAKESERIALPORT_H
#define FAKESERIALPORT_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ISerialDevice.h>
#include <yarp/os/Bottle.h>

#include <stdio.h>

#include "FakeSerialPort_ParamsParser.h"

using namespace yarp::os;

/**
 * @ingroup dev_impl_other
 *
 * \brief `fakeSerialPort`: A fake basic Serial Communications Link (RS232, USB).
 *
 * Parameters required by this device are shown in class: FakeSerialPort_ParamsParser
 * Beware: all parameters parsed by fakeSerialPort are actually ignored because it is a fake device
 * which does not connect to any real hardware.
 */
class FakeSerialPort :
        public yarp::dev::DeviceDriver,
        public yarp::dev::ISerialDevice,
        public FakeSerialPort_ParamsParser
{
private:
    FakeSerialPort(const FakeSerialPort&);

    bool verbose = true;

public:
    FakeSerialPort();

    virtual ~FakeSerialPort();

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    bool send(const Bottle& msg) override;
    bool send(const char *msg, size_t size) override;
    bool receive(Bottle& msg) override;
    int  receiveChar(char& chr) override;
    int  receiveBytes(unsigned char* bytes, const int size) override;
    int  receiveLine(char* line, const int MaxLineLength) override;
    bool setDTR(bool value) override;
    int  flush() override;
};

#endif
