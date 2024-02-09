/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeSerialPort.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>

#include <cstdio>
#include <cstdlib>

using namespace yarp::os;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(FAKESERIALPORT, "yarp.device.FakeSerialPort")
}

FakeSerialPort::FakeSerialPort() {
    //system_resources = (SerialHandler*) new SerialHandler();
    line_terminator_char1 = '\r';
    line_terminator_char2 = '\n';
}

FakeSerialPort::~FakeSerialPort()
{
    close();
}

bool FakeSerialPort::open(yarp::os::Searchable& config)
{
#if 0
    SerialDeviceDriverSettings config2;
    strcpy(config2.CommChannel, config.check("comport",Value("COM3"),"name of the serial channel").asString().c_str());
    this->verbose = (config.check("verbose",Value(1),"Specifies if the device is in verbose mode (0/1).").asInt32())>0;
    config2.SerialParams.baudrate = config.check("baudrate",Value(9600),"Specifies the baudrate at which the communication port operates.").asInt32();
    config2.SerialParams.xonlim = config.check("xonlim",Value(0),"Specifies the minimum number of bytes in input buffer before XON char is sent. Negative value indicates that default value should be used (Win32)").asInt32();
    config2.SerialParams.xofflim = config.check("xofflim",Value(0),"Specifies the maximum number of bytes in input buffer before XOFF char is sent. Negative value indicates that default value should be used (Win32). ").asInt32();
    //RANDAZ: as far as I undesrood, the exit condition for recv() function is NOT readmincharacters || readtimeoutmsec. It is readmincharacters && readtimeoutmsec.
    //On Linux. if readmincharacters params is set !=0, recv() may still block even if readtimeoutmsec is expired.
    //On Win32, for unknown reason, readmincharacters seems to be ignored, so recv () returns after readtimeoutmsec. Maybe readmincharacters is used if readtimeoutmsec is set to -1?
    config2.SerialParams.readmincharacters = config.check("readmincharacters",Value(1),"Specifies the minimum number of characters for non-canonical read (POSIX).").asInt32();
    config2.SerialParams.readtimeoutmsec = config.check("readtimeoutmsec",Value(100),"Specifies the time to wait before returning from read. Negative value means infinite timeout.").asInt32();
    // config2.SerialParams.parityenb = config.check("parityenb",Value(0),"Enable/disable parity checking.").asInt32();
    std::string temp = config.check("paritymode",Value("EVEN"),"Specifies the parity mode (EVEN, ODD, NONE). POSIX supports even and odd parity. Additionally Win32 supports mark and space parity modes.").asString();
    config2.SerialParams.paritymode = temp.c_str();
    config2.SerialParams.ctsenb = config.check("ctsenb",Value(0),"Enable & set CTS mode. Note that RTS & CTS are enabled/disabled together on some systems (RTS/CTS is enabled if either <code>ctsenb</code> or <code>rtsenb</code> is set).").asInt32();
    config2.SerialParams.rtsenb = config.check("rtsenb",Value(0),"Enable & set RTS mode. Note that RTS & CTS are enabled/disabled together on some systems (RTS/CTS is enabled if either <code>ctsenb</code> or <code>rtsenb</code> is set).\n- 0 = Disable RTS.\n- 1 = Enable RTS.\n- 2 = Enable RTS flow-control handshaking (Win32).\n- 3 = Specifies that RTS line will be high if bytes are available for transmission.\nAfter transmission RTS will be low (Win32).").asInt32();
    config2.SerialParams.xinenb = config.check("xinenb",Value(0),"Enable/disable software flow control on input.").asInt32();
    config2.SerialParams.xoutenb = config.check("xoutenb",Value(0),"Enable/disable software flow control on output.").asInt32();
    config2.SerialParams.modem = config.check("modem",Value(0),"Specifies if device is a modem (POSIX). If not set modem status lines are ignored. ").asInt32();
    config2.SerialParams.rcvenb = config.check("rcvenb",Value(0),"Enable/disable receiver (POSIX).").asInt32();
    config2.SerialParams.dsrenb = config.check("dsrenb",Value(0),"Controls whether DSR is disabled or enabled (Win32).").asInt32();
    config2.SerialParams.dtrdisable = config.check("dtrdisable",Value(0),"Controls whether DTR is disabled or enabled.").asInt32();
    config2.SerialParams.databits = config.check("databits",Value(7),"Data bits. Valid values 5, 6, 7 and 8 data bits. Additionally Win32 supports 4 data bits.").asInt32();
    config2.SerialParams.stopbits = config.check("stopbits",Value(1),"Stop bits. Valid values are 1 and 2.").asInt32();

    if (config.check("line_terminator_char1", "line terminator character for receiveLine(), default '\r'")) {
        line_terminator_char1 = config.find("line_terminator_char1").asInt32();
    }

    if (config.check("line_terminator_char2", "line terminator character for receiveLine(), default '\n'")) {
        line_terminator_char2 = config.find("line_terminator_char2").asInt32();
    }
#endif
    return true;
}

bool FakeSerialPort::close()
{
    return true;
}

bool FakeSerialPort::setDTR(bool value)
{
    return true;
}

bool FakeSerialPort::send(const Bottle& msg)
{
    if (msg.size() > 0)
    {
        int message_size = msg.get(0).asString().length();

        if (message_size > 0)
        {
            if (verbose)
            {
                yCDebug(FAKESERIALPORT, "Sending string: %s", msg.get(0).asString().c_str());
            }
        }
        else
        {
            if (verbose)
            {
                yCDebug(FAKESERIALPORT, "The input command bottle contains an empty string.");
            }
           return false;
        }
    }
    else
    {
        if (verbose)
        {
            yCDebug(FAKESERIALPORT, "The input command bottle is empty. \n");
        }
        return false;
    }

    return true;
}

bool FakeSerialPort::send(const char *msg, size_t size)
{
    if (size > 0)
    {
        if (verbose)
        {
            yCDebug(FAKESERIALPORT, "Sending string: %s", msg);
        }

        // Write message in the serial device
        size_t bytes_written = size;

        if (bytes_written == -1)
        {
            yCError(FAKESERIALPORT, "Unable to write to serial port");
            return false;
        }
    }
    else
    {
        if (verbose) {
            yCDebug(FAKESERIALPORT, "The input message is empty. \n");
        }
        return false;
    }

    yCInfo (FAKESERIALPORT, "sent command: %s \n",msg);
    return true;
}

int FakeSerialPort::receiveChar(char& c)
{
    char chr='c';

    size_t bytes_read = 1;

    if (bytes_read == -1)
    {
        yCError(FAKESERIALPORT, "Error in SerialDeviceDriver::receive()");
        return 0;
    }

    if (bytes_read == 0)
    {
        return 0;
    }

    c=chr;
    return 1;
}

int  FakeSerialPort::flush()
{
    return 1;
}

int FakeSerialPort::receiveBytes(unsigned char* bytes, const int size)
{
    for (size_t i=0; i< size; i++)
    bytes[i]='0'+i;
    return size;
}

int FakeSerialPort::receiveLine(char* buffer, const int MaxLineLength)
{
    int i;
    for (i = 0; i < MaxLineLength -1; ++i)
    {
        char recv_ch;
        int n = receiveChar(recv_ch);
        if (n <= 0)
        {
            //this invalidates the whole line, because no line terminator \n was found
            return 0;

            //use this commented code here if you do NOT want to invalidate the line
            //buffer[i] = '\0';
            //return i;
        }
        if ((recv_ch == line_terminator_char1) || (recv_ch == line_terminator_char2))
        {
            buffer[i] = recv_ch;
            i++;
            break;
        }
        buffer[i] = recv_ch;
     }
     buffer[i] = '\0';
     return i;
}

bool FakeSerialPort::receive(Bottle& msg)
{
    char message[10] = "123456789";

    //this function call blocks
    size_t bytes_read = 9;

    if (bytes_read == -1)
    {
        yCError(FAKESERIALPORT, "Error in SerialDeviceDriver::receive()");
        return false;
    }

    if (bytes_read == 0) { //nothing there
        return true;
    }

    message[bytes_read] = 0;

    if (verbose)
    {
        yCDebug(FAKESERIALPORT, "Data received from serial device: %s", message);
    }


    // Put message in the bottle
    msg.addString(message);

    return true;
}
