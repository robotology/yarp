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

ReturnValue FakeSerialPort::setDTR(bool value)
{
    return ReturnValue_ok;
}

ReturnValue FakeSerialPort::sendString(const std::string& msg)
{
    size_t message_size = msg.size();

    if (message_size > 0)
    {
        if (verbose)
        {
            yCDebug(FAKESERIALPORT, "Sending string: %s", msg.c_str());
        }
    }
    else
    {
        if (verbose)
        {
            yCDebug(FAKESERIALPORT, "The string contains an empty string.");
        }
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
    }

    return ReturnValue_ok;
}

ReturnValue FakeSerialPort::sendByte(unsigned char byt)
{
    yCInfo(FAKESERIALPORT, "sent byte : %c \n", byt);
    return ReturnValue_ok;
}

ReturnValue FakeSerialPort::sendBytes(const std::vector<unsigned char>& msg)
{
    if (msg.size() > 0)
    {
        if (verbose)
        {
            yCDebug(FAKESERIALPORT, "Sending string: %s", msg);
        }

        // Write message in the serial device
        size_t bytes_written = msg.size();

        if (bytes_written == -1)
        {
            yCError(FAKESERIALPORT, "Unable to write to serial port");
            return ReturnValue::return_code::return_value_error_method_failed;
        }
    }
    else
    {
        if (verbose) {
            yCDebug(FAKESERIALPORT, "The input message is empty. \n");
        }
        return ReturnValue::return_code::return_value_error_method_failed;
    }

    yCInfo (FAKESERIALPORT, "sent command: %s \n",msg);
    return ReturnValue_ok;
}

ReturnValue FakeSerialPort::receiveByte(unsigned char& c)
{
    char chr='R';

    size_t bytes_read = 1;

    if (bytes_read == -1)
    {
        yCError(FAKESERIALPORT, "Error in SerialDeviceDriver::receive()");
        return ReturnValue::return_code::return_value_error_method_failed;
    }

    if (bytes_read == 0)
    {
        return ReturnValue::return_code::return_value_error_method_failed;
    }

    c=chr;
    return ReturnValue_ok;
}

ReturnValue FakeSerialPort::flush()
{
    size_t dummy_counter = 0;
    return flush(dummy_counter);
}

ReturnValue  FakeSerialPort::flush(size_t& flushed)
{
    flushed = 3;
    return ReturnValue_ok;
}

ReturnValue FakeSerialPort::receiveBytes(std::vector<unsigned char>& line, const int MaxSize)
{
    line.clear();
    line.resize(1000);
    for (size_t i = 0; i < MaxSize; i++)
    {
        line[i]= ('0' + i);
    }
    return ReturnValue_ok;
}

ReturnValue FakeSerialPort::receiveLine(std::vector<char>& line, const int MaxLineLength)
{
    int i;
    for (i = 0; i < MaxLineLength -1; ++i)
    {
        unsigned char recv_ch;
        auto ret = receiveByte(recv_ch);
        if (!ret)
        {
            //this invalidates the whole line, because no line terminator \n was found
            return ReturnValue::return_code::return_value_error_method_failed;

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

ReturnValue FakeSerialPort::receiveString(std::string& msg)
{
    char message[10] = "123456789";

    //this function call blocks
    size_t bytes_read = 9;

    if (bytes_read == -1)
    {
        yCError(FAKESERIALPORT, "Error in SerialDeviceDriver::receive()");
        return ReturnValue::return_code::return_value_error_method_failed;
    }

    if (bytes_read == 0) { //nothing there
        return ReturnValue_ok;
    }

    message[bytes_read] = 0;

    if (verbose)
    {
        yCDebug(FAKESERIALPORT, "Data received from serial device: %s", message);
    }


    // Return the message
    msg=message;

    return ReturnValue_ok;
}
