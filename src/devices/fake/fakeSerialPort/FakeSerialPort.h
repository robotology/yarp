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

class SerialDeviceDriverSettings
{
public:
    char CommChannel[100]; // Contains the name of the com port "COM1", "COM2" (windows) or "/etc/stty0", "/dev/stty1" (linux), etc...
    /** Serial_Params contains the following variables: */
    /** int baudrate; Specifies the baudrate at which the communication port operates. */
    /** int xonlim; Specifies the minimum number of bytes in input buffer before XON char
        is sent. Negative value indicates that default value should
        be used (Win32). */
    /** int xofflim; Specifies the maximum number of bytes in input buffer before XOFF char
        is sent. Negative value indicates that default value should
        be used (Win32). */
    /** unsigned int readmincharacters; Specifies the minimum number of characters for non-canonical
        read (POSIX). */
    /** int readtimeoutmsec; Specifies the time to wait before returning from read. Negative value
        means infinite timeout. */
    /** const char *paritymode; Specifies the parity mode (EVEN, ODD, NONE). POSIX supports "even" and "odd" parity.
        Additionally Win32 supports "mark" and "space" parity modes. */
    /** bool ctsenb; Enable & set CTS mode. Note that RTS & CTS are enabled/disabled
        together on some systems (RTS/CTS is enabled if either
        <code>ctsenb</code> or <code>rtsenb</code> is set). */
    /** int rtsenb; Enable & set RTS mode. Note that RTS & CTS are enabled/disabled
        together on some systems (RTS/CTS is enabled if either
        <code>ctsenb</code> or <code>rtsenb</code> is set).
        - 0 = Disable RTS.
        - 1 = Enable RTS.
        - 2 = Enable RTS flow-control handshaking (Win32).
        - 3 = Specifies that RTS line will be high if bytes are available
              for transmission. After transmission RTS will be low (Win32). */
    /** bool xinenb; Enable/disable software flow control on input. */
    /** bool xoutenb; Enable/disable software flow control on output. */
    /** bool modem; Specifies if device is a modem (POSIX). If not set modem status
        lines are ignored. */
    /** bool rcvenb; Enable/disable receiver (POSIX). */
    /** bool dsrenb; Controls whether DSR is disabled or enabled (Win32). */
    /** bool dtrdisable; Controls whether DTR is disabled or enabled. */
    /** unsigned char databits; Data bits. Valid values 5, 6, 7 and 8 data bits.
        Additionally Win32 supports 4 data bits. */
    /** unsigned char stopbits; Stop bits. Valid values are 1 and 2. */
};

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
    void operator=(const FakeSerialPort&);

    bool verbose = true;
    char line_terminator_char1;
    char line_terminator_char2;

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
