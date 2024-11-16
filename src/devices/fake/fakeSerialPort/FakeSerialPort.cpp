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
}

FakeSerialPort::~FakeSerialPort()
{
    close();
}

bool FakeSerialPort::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) {return false;}

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
        if ((recv_ch == m_line_terminator_char1) || (recv_ch == m_line_terminator_char2))
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
